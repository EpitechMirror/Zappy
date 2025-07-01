#!/usr/bin/python3
##
## ai_manager.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Wed Jun 25 11:39:38 AM 2025 adrien.marette@epitech.eu
## Last update Mon Jun 29 4:49:05 PM 2025 adrien.marette@epitech.eu
##

import os
import sys
import time
import signal
import socket
import multiprocessing
from typing import List, Dict, Set
import argparse
from main_ai import ZappyAI

class ZappyAIManager:
    """Manager for spawning and controlling multiple Zappy AI clients"""
    
    def __init__(self, port: int, team_name: str, hostname: str = "localhost", max_processes: int = 10):
        self.port = port
        self.team_name = team_name
        self.hostname = hostname
        self.max_processes = max_processes
        
        # Process management
        self.active_processes: Dict[int, multiprocessing.Process] = {}
        self.process_counter = 0
        self.total_spawned = 0
        
        # Server information
        self.available_slots = None
        self.world_width = None
        self.world_height = None
        
        # Control flags
        self.running = True
        self.spawn_interval = 2.0  # Time between spawn attempts
        self.last_spawn_attempt = 0
        
        # Statistics
        self.stats = {
            'total_spawned': 0,
            'active_ais': 0,
            'completed_ais': 0,
            'failed_spawns': 0,
            'successful_spawns': 0
        }
        
        # Setup signal handling
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        """Handle shutdown signals gracefully"""
        print(f"\n🛑 Received signal {signum}, shutting down AI Manager...")
        self.running = False
        self.cleanup_all_processes()
        sys.exit(0)
    
    def get_server_info(self) -> bool:
        """Connect to server to get available slots and world dimensions"""
        try:
            # Create temporary connection to get server info
            temp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            temp_socket.settimeout(10.0)  # 10 second timeout
            temp_socket.connect((self.hostname, self.port))
            
            # Follow handshake protocol
            welcome = temp_socket.recv(1024).decode().strip()
            print(f"🔍 Debug: Received welcome: '{welcome}'")
            if not welcome.startswith("WELCOME"):
                print(f"❌ Unexpected server response: {welcome}")
                return False
            
            # Send team name
            temp_socket.send(f"{self.team_name}\n".encode())
            print(f"🔍 Debug: Sent team name: '{self.team_name}'")
            
            # Get server response (slots + dimensions in one or multiple messages)
            full_response = temp_socket.recv(1024).decode().strip()
            print(f"🔍 Debug: Full server response: '{full_response}'")
            lines = full_response.split('\n')
            
            # Parse available slots - skip welcome message if present
            slots_line_idx = 0
            for i, line in enumerate(lines):
                try:
                    # Try to parse as integer
                    self.available_slots = int(line.strip())
                    slots_line_idx = i
                    break
                except ValueError:
                    continue  # Skip non-numeric lines
            
            if self.available_slots is None:
                print(f"❌ Invalid slots response: {full_response}")
                return False
            
            # Parse world dimensions (next line after slots)
            try:
                dims_line_idx = slots_line_idx + 1
                if dims_line_idx < len(lines):
                    # Dimensions are in the same response
                    dims_parts = lines[dims_line_idx].split()
                else:
                    # Need to read another message for dimensions
                    dims_response = temp_socket.recv(1024).decode().strip()
                    dims_parts = dims_response.split()
                
                self.world_width = int(dims_parts[0])
                self.world_height = int(dims_parts[1])
            except (ValueError, IndexError):
                print(f"❌ Invalid dimensions response: {full_response}")
                return False
            
            temp_socket.close()
            
            print(f"✅ Server info obtained:")
            print(f"   Available slots: {self.available_slots}")
            print(f"   World size: {self.world_width}x{self.world_height}")
            
            return True
            
        except Exception as e:
            print(f"❌ Failed to get server info: {e}")
            return False
    
    def spawn_ai_process(self) -> bool:
        """Spawn a new AI client process"""
        try:
            # Check if we have room for more processes
            if len(self.active_processes) >= self.max_processes:
                print(f"⚠️  Maximum process limit reached ({self.max_processes})")
                return False
            
            # Check if there are available slots
            if self.available_slots is not None and len(self.active_processes) >= self.available_slots:
                print(f"⚠️  No more team slots available ({self.available_slots})")
                return False
            
            # Create a unique process identifier
            self.process_counter += 1
            process_id = self.process_counter
            
            # Create the AI process
            process = multiprocessing.Process(
                target=self.run_ai_client,
                args=(process_id,),
                name=f"ZappyAI-{self.team_name}-{process_id}"
            )
            
            process.start()
            self.active_processes[process_id] = process
            self.stats['total_spawned'] += 1
            self.stats['successful_spawns'] += 1
            
            print(f"🚀 Spawned AI process #{process_id} (PID: {process.pid})")
            print(f"📊 Active processes: {len(self.active_processes)}/{self.max_processes}")
            
            return True
            
        except Exception as e:
            print(f"❌ Failed to spawn AI process: {e}")
            self.stats['failed_spawns'] += 1
            return False
    
    def run_ai_client(self, process_id: int):
        """Run a single AI client in its own process"""
        try:
            print(f"🤖 AI Process #{process_id} starting...")
            
            # Create AI with unique identifier in logs
            ai = ZappyAI(self.port, self.team_name, self.hostname)
            
            # Modify AI to include process ID in its output
            original_print = print
            def ai_print(*args, **kwargs):
                original_print(f"[AI-{process_id}]", *args, **kwargs)
            
            # Replace print function for this AI instance
            import builtins
            builtins.print = ai_print
            
            # Run the AI
            ai.run()
            
            print(f"✅ AI Process #{process_id} completed successfully")
            
        except Exception as e:
            print(f"❌ AI Process #{process_id} failed: {e}")
            import traceback
            traceback.print_exc()
    
    def cleanup_completed_processes(self):
        """Remove completed processes from active list"""
        completed_processes = []
        
        for process_id, process in self.active_processes.items():
            if not process.is_alive():
                completed_processes.append(process_id)
                process.join(timeout=1.0)  # Wait briefly for cleanup
                
                if process.exitcode == 0:
                    self.stats['completed_ais'] += 1
                    print(f"✅ AI Process #{process_id} completed successfully")
                else:
                    print(f"❌ AI Process #{process_id} exited with code {process.exitcode}")
        
        # Remove completed processes
        for process_id in completed_processes:
            del self.active_processes[process_id]
        
        self.stats['active_ais'] = len(self.active_processes)
    
    def cleanup_all_processes(self):
        """Terminate all active processes"""
        print(f"🧹 Cleaning up {len(self.active_processes)} active processes...")
        
        for process_id, process in self.active_processes.items():
            if process.is_alive():
                print(f"   Terminating AI Process #{process_id}...")
                process.terminate()
                process.join(timeout=5.0)
                
                if process.is_alive():
                    print(f"   Force killing AI Process #{process_id}...")
                    process.kill()
                    process.join()
        
        self.active_processes.clear()
        print("✅ All processes cleaned up")
    
    def should_spawn_new_ai(self) -> bool:
        """Determine if we should spawn a new AI process"""
        current_time = time.time()
        
        # Check spawn interval
        if current_time - self.last_spawn_attempt < self.spawn_interval:
            return False
        
        # Check if we have available slots
        if self.available_slots is not None:
            if len(self.active_processes) >= self.available_slots:
                return False
        
        # Check process limit
        if len(self.active_processes) >= self.max_processes:
            return False
        
        # Check if server might have new eggs available
        # Spawn aggressively at the beginning, then more conservatively
        if len(self.active_processes) == 0:
            return True  # Always spawn first AI
        elif len(self.active_processes) < 3:
            return True  # Spawn up to 3 quickly
        elif current_time - self.last_spawn_attempt > 30.0:
            return True  # Try every 30 seconds after that
        
        return False
    
    def print_stats(self):
        """Print current statistics"""
        print(f"\n📊 AI Manager Statistics:")
        print(f"   Total spawned: {self.stats['total_spawned']}")
        print(f"   Currently active: {len(self.active_processes)}")
        print(f"   Completed: {self.stats['completed_ais']}")
        print(f"   Successful spawns: {self.stats['successful_spawns']}")
        print(f"   Failed spawns: {self.stats['failed_spawns']}")
        if self.available_slots:
            print(f"   Available slots: {self.available_slots}")
        print(f"   Max processes: {self.max_processes}")
    
    def monitor_and_fork(self):
        """Main monitoring loop for spawning AI processes"""
        print(f"🎮 Starting Zappy AI Manager for team '{self.team_name}'")
        print(f"🌐 Target server: {self.hostname}:{self.port}")
        print(f"📈 Max processes: {self.max_processes}")
        
        # Get initial server information
        if not self.get_server_info():
            print("❌ Failed to get server information, exiting...")
            return
        
        # Spawn initial AI immediately
        print("\n🚀 Spawning initial AI client...")
        self.spawn_ai_process()
        self.last_spawn_attempt = time.time()
        
        # Main monitoring loop
        last_stats_print = time.time()
        
        while self.running:
            try:
                # Clean up completed processes
                self.cleanup_completed_processes()
                
                # Check if we should spawn new AI
                if self.should_spawn_new_ai():
                    self.spawn_ai_process()
                    self.last_spawn_attempt = time.time()
                
                # Print stats periodically
                current_time = time.time()
                if current_time - last_stats_print > 60.0:  # Every minute
                    self.print_stats()
                    last_stats_print = current_time
                
                # Brief sleep to prevent busy waiting
                time.sleep(1.0)
                
            except KeyboardInterrupt:
                print("\n🛑 Keyboard interrupt received")
                break
            except Exception as e:
                print(f"❌ Manager error: {e}")
                time.sleep(5.0)  # Wait before continuing
        
        # Final cleanup
        print("\n🏁 Shutting down AI Manager...")
        self.cleanup_all_processes()
        self.print_stats()
    
    def run(self):
        """Main entry point"""
        try:
            self.monitor_and_fork()
        except Exception as e:
            print(f"❌ Fatal manager error: {e}")
            import traceback
            traceback.print_exc()
        finally:
            self.cleanup_all_processes()


def main():
    """Main function for AI Manager"""
    parser = argparse.ArgumentParser(description="Zappy AI Manager - Spawn multiple AI clients")
    parser.add_argument("-p", "--port", type=int, required=True, help="Server port number")
    parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
    parser.add_argument("-H", "--hostname", type=str, default="localhost", help="Server hostname")
    parser.add_argument("-m", "--max-processes", type=int, default=10, help="Maximum number of AI processes")
    parser.add_argument("--spawn-interval", type=float, default=2.0, help="Minimum time between spawns (seconds)")
    
    args = parser.parse_args()
    
    print("🤖 Zappy AI Manager - Multi-Process AI Spawner")
    print("=" * 60)
    print(f"🌐 Target: {args.hostname}:{args.port}")
    print(f"👥 Team: {args.name}")
    print(f"📈 Max processes: {args.max_processes}")
    print(f"⏱️  Spawn interval: {args.spawn_interval}s")
    
    # Set multiprocessing start method
    multiprocessing.set_start_method('spawn', force=True)
    
    # Create and run manager
    manager = ZappyAIManager(
        port=args.port,
        team_name=args.name,
        hostname=args.hostname,
        max_processes=args.max_processes
    )
    
    manager.spawn_interval = args.spawn_interval
    manager.run()


if __name__ == "__main__":
    main()
