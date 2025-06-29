##
## main_ai.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Wed Jun 25 11:39:38 AM 2025 adrien.marette@epitech.eu
## Last update Mon Jun 29 4:49:04 PM 2025 adrien.marette@epitech.eu
##

import json
import sys
import time
import socket
import random
import numpy as np
from collections import deque
from typing import List, Dict, Any
from MemorySystem import MemorySystem
from NeuralNetwork import NeuralNetwork
from ThinkingModule import ThinkingModule
from Classes import GameState, ActionType, ResourceType, Position, Experience

class ZappyAI:
    """Zappy AI with neural networks, learning, and advanced thinking"""
    
    def __init__(self, port: int, team_name: str, hostname: str = "localhost"):
        self.port = port
        self.team_name = team_name
        self.hostname = hostname
        self.socket = None
        self.state = GameState()
        self.command_queue = []
        self.last_look_result = []
        
        # Fork and team management
        self.available_slots = None
        self.world_width = None
        self.world_height = None
        self.last_fork_attempt = 0
        self.fork_cooldown = 60.0  # Wait 60 seconds between fork attempts
        self.should_fork_early = True  # Fork aggressively at the beginning
        
        # AI Components
        self.neural_network = NeuralNetwork(
            input_size=50,  # State representation size
            hidden_sizes=[128, 64, 32],
            output_size=len(ActionType),
            learning_rate=0.001
        )
        self.memory = MemorySystem()
        self.thinking_module = ThinkingModule()
        
        # Learning parameters
        self.epsilon = 0.3  # Exploration rate
        self.epsilon_decay = 0.995
        self.epsilon_min = 0.01
        self.learning_enabled = True
        self.model_path = f"zappy_ai_model_{team_name}.pkl"
        
        # Load existing model if available
        self.neural_network.load(self.model_path)
        
        # Performance tracking
        self.performance_metrics = {
            'total_rewards': 0,
            'successful_actions': 0,
            'failed_actions': 0,
            'elevations_completed': 0,
            'resources_collected': 0,
            'forks_attempted': 0,
            'forks_successful': 0
        }
        
        # Action history for pattern recognition
        self.action_history = deque(maxlen=1000)
        self.reward_history = deque(maxlen=1000)
        
        # Fork-related attributes
        self.available_slots = None
        self.world_width = None
        self.world_height = None
        self.fork_cooldown = 60.0  # 60 seconds between fork attempts
        self.last_fork_attempt = 0
        self.should_fork_early = True  # Aggressive early game forking
        
    def connect(self):
        """Connect to the Zappy server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.hostname, self.port))

            welcome = self.receive_message()
            if not welcome.startswith("WELCOME"):
                raise Exception(f"Expected WELCOME, got: {welcome}")

            self.send_message(self.team_name)
            
            # Get server info for fork planning
            server_response = self.receive_message()
            try:
                # Server sends welcome message, slots and dimensions in response
                lines = server_response.split('\n')
                
                # Find slots (first numeric line)
                slots_found = False
                dims_line_idx = -1
                for i, line in enumerate(lines):
                    try:
                        self.available_slots = int(line.strip())
                        print(f"Team slots available: {self.available_slots}")
                        slots_found = True
                        dims_line_idx = i + 1
                        break
                    except ValueError:
                        continue  # Skip non-numeric lines
                
                # Find dimensions (next line after slots)
                if slots_found and dims_line_idx < len(lines):
                    parts = lines[dims_line_idx].split()
                    self.world_width = int(parts[0])
                    self.world_height = int(parts[1])
                    print(f"World size: {self.world_width}x{self.world_height}")
                elif slots_found:
                    # Dimensions might be in a separate message
                    dims_response = self.receive_message()
                    parts = dims_response.split()
                    self.world_width = int(parts[0])
                    self.world_height = int(parts[1])
                    print(f"World size: {self.world_width}x{self.world_height}")
                    
            except (ValueError, IndexError) as e:
                print(f"Warning: Could not parse server response: {server_response}")
                # Continue anyway, these are just for optimization

            print(f"Enhanced AI connected successfully!")
            return True
        except Exception as e:
            print(f"Connection failed: {e}")
            return False
    
    def send_message(self, message: str):
        """Send a message to the server"""
        if self.socket:
            full_message = message + "\n"
            self.socket.send(full_message.encode())

    def receive_message(self) -> str:
        """Receive a message from the server"""
        if self.socket:
            data = self.socket.recv(1024).decode().strip()
            return data
        return ""
    
    def create_state_vector(self) -> np.array:
        """Create a numerical representation of the current state"""
        state_vector = []
        
        # Basic state information
        state_vector.extend([
            self.state.level / 7.0,  # Normalized level
            self.state.food_units / 100.0,  # Normalized food
            self.state.turn_count / 1000.0,  # Normalized turn count
            self.state.direction.value / 4.0,  # Normalized direction
        ])
        
        # Inventory (normalized)
        for resource in ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]:
            state_vector.append(self.state.inventory.get(resource, 0) / 10.0)
        
        # Vision information (flattened and normalized)
        vision_flat = []
        for tile in self.last_look_result[:9]:  # Take first 9 tiles
            tile_vector = [0] * 7  # 7 different resource types
            for item in tile:
                if item == "food":
                    tile_vector[0] = 1
                elif item == "linemate":
                    tile_vector[1] = 1
                elif item == "deraumere":
                    tile_vector[2] = 1
                elif item == "sibur":
                    tile_vector[3] = 1
                elif item == "mendiane":
                    tile_vector[4] = 1
                elif item == "phiras":
                    tile_vector[5] = 1
                elif item == "thystame":
                    tile_vector[6] = 1
            vision_flat.extend(tile_vector)
        
        # Pad vision if necessary
        while len(vision_flat) < 35:  # 9 tiles * 7 resources - 28, but we want 35 for round number
            vision_flat.append(0)
        
        state_vector.extend(vision_flat[:35])
        
        # Recent performance
        recent_rewards = list(self.reward_history)[-10:] if self.reward_history else [0]
        avg_recent_reward = sum(recent_rewards) / len(recent_rewards)
        state_vector.append(avg_recent_reward)
        
        # Pad to exact size
        while len(state_vector) < 50:
            state_vector.append(0)
        
        return np.array(state_vector[:50]).reshape(1, -1)
    
    def calculate_reward(self, action: ActionType, result: str, previous_state: GameState) -> float:
        """Calculate reward based on action and result"""
        reward = 0.0
        
        # Basic survival reward
        reward += 0.1
        
        # Food management
        food_diff = self.state.inventory.get('food', 0) - previous_state.inventory.get('food', 0)
        if food_diff > 0:
            reward += food_diff * 2.0  # Reward for gaining food
        elif self.state.inventory.get('food', 0) < 10:
            reward -= 5.0  # Penalty for low food
        
        # Resource collection
        for resource in ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]:
            diff = self.state.inventory.get(resource, 0) - previous_state.inventory.get(resource, 0)
            if diff > 0:
                reward += diff * 3.0  # Reward for collecting elevation resources
        
        # Level progression
        if self.state.level > previous_state.level:
            reward += 100.0  # Big reward for leveling up
            self.performance_metrics['elevations_completed'] += 1
        
        # Action-specific rewards
        if action == ActionType.LOOK and result.startswith('['):
            reward += 0.5  # Small reward for successful look
        elif action == ActionType.TAKE_RESOURCE and result == "ok":
            reward += 5.0
            self.performance_metrics['resources_collected'] += 1
        elif action == ActionType.FORK and result == "ok":
            reward += 20.0  # Good reward for successful fork
        elif result == "ko":
            reward -= 2.0  # Penalty for failed actions
            self.performance_metrics['failed_actions'] += 1
        elif result == "ok":
            reward += 1.0
            self.performance_metrics['successful_actions'] += 1
        
        # Death penalty
        if result == "dead":
            reward -= 100.0
        
        return reward
    
    def choose_action(self, state_vector: np.array) -> ActionType:
        """Choose action using neural network with epsilon-greedy exploration"""
        if random.random() < self.epsilon and self.learning_enabled:
            # Exploration: random action
            action_idx = random.randint(0, len(ActionType) - 1)
        else:
            # Exploitation: use neural network
            q_values = self.neural_network.predict(state_vector)
            action_idx = np.argmax(q_values[0])
        
        return list(ActionType)[action_idx]
    
    def think_and_decide(self) -> ActionType:
        """Advanced thinking and decision making"""
        # Check if we should fork first (high priority)
        if self.attempt_fork():
            return ActionType.FORK
        
        # Get current state representation
        state_vector = self.create_state_vector()
        
        # Think about the situation
        situation_analysis = self.thinking_module.analyze_situation(self.state, self.last_look_result)
        
        # Check if we have a successful strategy for this situation
        situation_key = f"level_{self.state.level}_food_{self.state.inventory.get('food', 0) // 10}"
        best_strategy = self.memory.get_best_strategy(situation_key)
        
        if best_strategy and random.random() > 0.3:  # Use strategy 70% of the time
            # Execute learned strategy
            action_idx = best_strategy[0] if best_strategy else 0
            return list(ActionType)[action_idx % len(ActionType)]
        
        # Use neural network for decision
        action = self.choose_action(state_vector)
        
        # Meta-thinking: adjust action based on context
        if situation_analysis['urgency_level'] >= 3:
            # Emergency mode - prioritize survival
            if self.state.inventory.get('food', 0) < 10:
                action = ActionType.LOOK  # Look for food
        
        return action
    
    def action_to_command(self, action: ActionType) -> str:
        """Convert action to command string"""
        if action == ActionType.MOVE_FORWARD:
            return "Forward"
        elif action == ActionType.TURN_LEFT:
            return "Left"
        elif action == ActionType.TURN_RIGHT:
            return "Right"
        elif action == ActionType.TAKE_RESOURCE:
            return "Take Resource"
        elif action == ActionType.LOOK:
            return "Look"
        elif action == ActionType.BROADCAST:
            return f"Broadcast ELEVATE_L{self.state.level}"
        elif action == ActionType.INCANTATION:
            return "Incantation"
        elif action == ActionType.FORK:
            return "Fork"
        elif action == ActionType.INVENTORY:
            return "Inventory"
        elif action == ActionType.EJECT:
            return "Eject"
        elif action == ActionType.DROP_RESOURCE:
            return "Set "
        else:
            return "Unknown Command"
    
    def execute_action(self, action: ActionType):
        """Execute the chosen action"""
        if action == ActionType.MOVE_FORWARD:
            self.send_command("Forward")
        elif action == ActionType.TURN_LEFT:
            self.send_command("Left")
        elif action == ActionType.TURN_RIGHT:
            self.send_command("Right")
        elif action == ActionType.TAKE_RESOURCE:
            # Intelligent resource taking
            if self.last_look_result and len(self.last_look_result) > 0:
                current_tile = self.last_look_result[0]
                for item in current_tile:
                    if item in ResourceType._value2member_map_:
                        self.send_command(f"Take {item}")
                        break
        elif action == ActionType.LOOK:
            self.send_command("Look")
        elif action == ActionType.BROADCAST:
            self.send_command(f"Broadcast ELEVATE_L{self.state.level}")
        elif action == ActionType.INCANTATION:
            self.send_command("Incantation")
        elif action == ActionType.FORK:
            self.send_command("Fork")
        elif action == ActionType.INVENTORY:
            self.send_command("Inventory")
    
    def send_command(self, command: str):
        """Send a command and add it to queue for response tracking"""
        self.send_message(command)
        self.command_queue.append(command)
        if len(self.command_queue) > 10:
            self.command_queue.pop(0)
    
    def learn_from_experience(self, experience: Experience):
        """Learn from a single experience"""
        if not self.learning_enabled:
            return
        
        try:
            # Add to memory
            self.memory.add_experience(experience)
            
            # Train neural network
            state = experience.state.reshape(1, -1)
            target = experience.reward
            if not experience.done:
                # Q-learning update
                next_q_values = self.neural_network.predict(experience.next_state.reshape(1, -1))
                target += 0.95 * np.max(next_q_values)  # Discount factor = 0.95
            
            # Update Q-values
            current_q_values = self.neural_network.predict(state)
            current_q_values[0][experience.action] = target
            
            # Train the network
            loss = self.neural_network.train(state, current_q_values)
            
            # Update performance metrics
            self.performance_metrics['total_rewards'] += experience.reward
            self.reward_history.append(experience.reward)
            
        except Exception as e:
            print(f"Learning error: {e}")
    
    def batch_learning(self):
        """Perform batch learning from memory"""
        if len(self.memory.experiences) < 32:
            return
        
        try:
            batch = self.memory.get_batch(32)
            states = np.array([exp.state for exp in batch])
            targets = np.zeros((len(batch), len(ActionType)))
            
            # Get current Q-values for all states in batch
            current_q_values = self.neural_network.predict(states)
            
            for i, exp in enumerate(batch):
                target = exp.reward
                if not exp.done:
                    next_q_values = self.neural_network.predict(exp.next_state.reshape(1, -1))
                    target += 0.95 * np.max(next_q_values)
                
                # Copy current Q-values and update only the taken action
                targets[i] = current_q_values[i].copy()
                targets[i][exp.action] = target
            
            # Batch training
            self.neural_network.train(states, targets)
            
        except Exception as e:
            print(f"Batch learning error: {e}")
    
    def process_response(self, response: str, previous_state: GameState, action: ActionType) -> bool:
        """Process server response and learn"""
        response = response.strip()
        
        # Handle fork success
        if action == ActionType.FORK and response == "ok":
            self.performance_metrics['forks_successful'] += 1
            print("🎉 Fork successful! New egg created.")
            # Reduce fork urgency after successful fork
            self.should_fork_early = False
        
        # Calculate reward
        reward = self.calculate_reward(action, response, previous_state)
        
        # Create experience
        current_state_vector = self.create_state_vector()
        if hasattr(self, 'previous_state_vector'):
            experience = Experience(
                state=self.previous_state_vector.flatten(),
                action=action.value,
                reward=reward,
                next_state=current_state_vector.flatten(),
                done=(response == "dead")
            )
            self.learn_from_experience(experience)
        
        self.previous_state_vector = current_state_vector
        
        # Process specific responses
        if response.startswith("[") and response.endswith("]"):
            self.last_look_result = self.parse_look_result(response)
        elif response.startswith("[") and "," in response:
            self.parse_inventory(response)
        elif response == "dead":
            print("AI died! Final performance:", self.performance_metrics)
            return False
        elif response.startswith("Current level:"):
            level_str = response.split(":")[1].strip()
            new_level = int(level_str)
            if new_level > self.state.level:
                print(f"🎉 LEVELED UP! New level: {new_level}")
                # Reset fork urgency on level up
                self.should_fork_early = True
            self.state.level = new_level
        
        # Update thinking module
        self.thinking_module.update_meta_knowledge(self.state, action, response)
        
        return True
    
    def parse_look_result(self, result: str) -> List[List[str]]:
        """Parse the look command result"""
        content = result.strip("[]")
        tiles = []
        current_tile = []

        i = 0
        while i < len(content):
            if content[i] == ",":
                tiles.append(current_tile)
                current_tile = []
                i += 1
                if i < len(content) and content[i] == " ":
                    i += 1
            else:
                end = i
                while end < len(content) and content[end] != ",":
                    end += 1

                tile_content = content[i:end].strip()
                if tile_content:
                    current_tile = tile_content.split()
                else:
                    current_tile = []
                i = end

        if current_tile or i == len(content):
            tiles.append(current_tile)

        return tiles
    
    def parse_inventory(self, inv_str: str):
        """Parse inventory response and update state"""
        content = inv_str.strip("[]")
        items = content.split(",")

        for item in items:
            parts = item.strip().split()
            if len(parts) == 2:
                resource, quantity = parts[0], int(parts[1])
                self.state.inventory[resource] = quantity
    
    def save_model(self):
        """Save the trained model"""
        self.neural_network.save(self.model_path)
        
        # Save performance metrics
        metrics_path = f"zappy_ai_metrics_{self.team_name}.json"
        with open(metrics_path, 'w') as f:
            json.dump(self.performance_metrics, f, indent=2)
        
        print(f"Model and metrics saved to {self.model_path} and {metrics_path}")
    
    def print_thinking_process(self, action: ActionType, situation_analysis: Dict[str, Any]):
        """Print AI's thinking process for debugging"""
        print(f"🧠 Turn {self.state.turn_count}: Thinking...")
        print(f"   Current Level: {self.state.level}")
        print(f"   Food: {self.state.inventory.get('food', 0)}")
        print(f"   Urgency: {situation_analysis['urgency_level']}")
        print(f"   Opportunities: {situation_analysis['opportunities'][:3]}")
        print(f"   Chosen Action: {action.name}")
        print(f"   Epsilon: {self.epsilon:.3f}")
        print(f"   Total Rewards: {self.performance_metrics['total_rewards']:.2f}")
    
    def adaptive_learning_adjustment(self):
        """Adaptively adjust learning parameters based on performance"""
        recent_rewards = list(self.reward_history)[-100:] if len(self.reward_history) >= 100 else list(self.reward_history)
        
        if recent_rewards:
            avg_reward = sum(recent_rewards) / len(recent_rewards)
            
            # Adjust exploration rate based on performance
            if avg_reward > 5.0:  # Good performance
                self.epsilon = max(self.epsilon * 0.99, self.epsilon_min)
            elif avg_reward < 0:  # Poor performance
                self.epsilon = min(self.epsilon * 1.01, 0.5)
            
            # Adjust learning rate
            if avg_reward < -10:  # Very poor performance
                self.neural_network.learning_rate = min(0.01, self.neural_network.learning_rate * 1.1)
            elif avg_reward > 10:  # Very good performance
                self.neural_network.learning_rate = max(0.0001, self.neural_network.learning_rate * 0.95)
    
    def should_fork(self) -> bool:
        """Determine if the AI should attempt to fork"""
        current_time = time.time()
        
        # Check cooldown
        if current_time - self.last_fork_attempt < self.fork_cooldown:
            return False
        
        # Early game: fork aggressively if we have good food
        if self.should_fork_early and self.state.turn_count < 500:
            if self.state.inventory.get('food', 0) > 50:
                return True
        
        # Mid/late game: fork when we have excess resources
        if self.state.inventory.get('food', 0) > 100:
            # Check if we have excess stones for our current level
            required_stones = self.get_required_stones_for_level(self.state.level + 1)
            current_stones = sum(self.state.inventory.get(stone, 0) for stone in required_stones.keys())
            
            if current_stones > sum(required_stones.values()) * 2:  # Have 2x what we need
                return True
        
        # Fork if we're high level and stable
        if self.state.level >= 3 and self.state.inventory.get('food', 0) > 80:
            return True
        
        return False
    
    def get_required_stones_for_level(self, level: int) -> dict:
        """Get required stones for elevation to a specific level"""
        requirements = {
            2: {"linemate": 1},
            3: {"linemate": 1, "deraumere": 1, "sibur": 1},
            4: {"linemate": 2, "sibur": 1, "phiras": 2},
            5: {"linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 3},
            6: {"linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 1, "phiras": 2},
            7: {"linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 1, "phiras": 1, "thystame": 1},
            8: {"linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}
        }
        return requirements.get(level, {})
    
    def attempt_fork(self):
        """Attempt to fork if conditions are right"""
        if self.should_fork():
            self.last_fork_attempt = time.time()
            self.performance_metrics['forks_attempted'] += 1
            print(f"🍴 Attempting to fork! (Food: {self.state.inventory.get('food', 0)}, Level: {self.state.level})")
            self.send_command("Fork")
            return True
        return False
    
    def run(self):
        """Enhanced main game loop with learning and thinking"""
        if not self.connect():
            return

        print("🚀 Starting Enhanced Zappy AI with Neural Networks and Learning...")
        print(f"📊 Initial Performance Metrics: {self.performance_metrics}")

        # Initialize
        self.send_command("Inventory")
        self.send_command("Look")
        
        # Wait for initial responses
        time.sleep(0.5)
        
        turn_counter = 0
        last_batch_learning = 0
        last_save = 0
        
        try:
            while True:
                turn_counter += 1
                self.state.turn_count = turn_counter
                
                # Store previous state for learning
                previous_state = GameState(
                    level=self.state.level,
                    position=Position(self.state.position.x, self.state.position.y),
                    direction=self.state.direction,
                    inventory=self.state.inventory.copy(),
                    food_units=self.state.food_units,
                    turn_count=self.state.turn_count
                )
                
                # Think and decide on action
                situation_analysis = self.thinking_module.analyze_situation(self.state, self.last_look_result)
                action = self.think_and_decide()
                
                # Store action in history
                self.action_history.append(action)
                
                # Print thinking process every 10 turns
                if turn_counter % 10 == 0:
                    self.print_thinking_process(action, situation_analysis)
                
                # Execute action
                self.execute_action(action)
                
                # Receive and process response
                try:
                    response = self.receive_message()
                    if response:
                        if not self.process_response(response, previous_state, action):
                            break
                except socket.timeout:
                    pass
                
                # Periodic batch learning
                if turn_counter - last_batch_learning >= 50:
                    self.batch_learning()
                    last_batch_learning = turn_counter
                
                # Adaptive learning adjustment
                if turn_counter % 100 == 0:
                    self.adaptive_learning_adjustment()
                
                # Decay exploration rate
                if self.epsilon > self.epsilon_min:
                    self.epsilon *= self.epsilon_decay
                
                # Periodic model saving
                if turn_counter - last_save >= 500:
                    self.save_model()
                    last_save = turn_counter
                    print(f"📈 Turn {turn_counter} - Performance Update:")
                    print(f"   Level: {self.state.level}")
                    print(f"   Total Rewards: {self.performance_metrics['total_rewards']:.2f}")
                    print(f"   Success Rate: {self.performance_metrics['successful_actions'] / max(1, self.performance_metrics['successful_actions'] + self.performance_metrics['failed_actions']) * 100:.1f}%")
                    print(f"   Resources Collected: {self.performance_metrics['resources_collected']}")
                    print(f"   Elevations: {self.performance_metrics['elevations_completed']}")
                    print(f"   Forks: {self.performance_metrics['forks_successful']}/{self.performance_metrics['forks_attempted']}")
                    print(f"   Food: {self.state.inventory.get('food', 0)}")
                    print(f"   Available Slots: {self.available_slots}")
                
                # Short sleep to prevent overwhelming the server
                time.sleep(0.05)

        except KeyboardInterrupt:
            print("\n🛑 AI stopped by user")
        except Exception as e:
            print(f"❌ AI error: {e}")
            import traceback
            traceback.print_exc()
        finally:
            # Final save and cleanup
            self.save_model()
            print("🏁 Final Performance Metrics:")
            for key, value in self.performance_metrics.items():
                print(f"   {key}: {value}")
            
            if self.socket:
                self.socket.close()


def main():
    """Main function for running AI from command line"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Zappy AI Client")
    parser.add_argument("-p", "--port", type=int, required=True, help="Server port number")
    parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
    parser.add_argument("-H", "--hostname", type=str, default="localhost", help="Server hostname")
    
    args = parser.parse_args()
    
    # Create and run AI
    ai = ZappyAI(args.port, args.name, args.hostname)
    if ai.connect():
        ai.run()
    else:
        print(f"❌ Failed to connect to server {args.hostname}:{args.port}")
        sys.exit(1)


if __name__ == "__main__":
    main()