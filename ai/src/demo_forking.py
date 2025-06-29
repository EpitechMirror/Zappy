#!/usr/bin/python3
##
## demo_forking.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Mon Jun 29 2025
## Last update Mon Jun 29 3:46:51 PM 2025 adrien.marette@epitech.eu
##

"""
Demo script showing the enhanced Zappy AI forking capabilities
"""

import sys
import time
import signal
import argparse
from ai_manager import ZappyAIManager

def demo_basic_forking():
    """Demo basic forking functionality"""
    print("🎮 DEMO: Basic AI Forking")
    print("=" * 60)
    print("This demo shows how the AI manager spawns multiple processes")
    print("to maximize team presence on the server.")
    print()
    
    # Create manager with limited processes for demo
    manager = ZappyAIManager(
        port=4242,
        team_name="DemoTeam",
        hostname="localhost",
        max_processes=3
    )
    
    print("📊 Initial State:")
    manager.print_stats()
    
    print("\n🔄 Simulating server connection...")
    # Note: This would normally connect to a real server
    print("⚠️  For this demo, connect to a real Zappy server on port 4242")
    print("   Example: ./zappy_server -p 4242 -x 10 -y 10 -n DemoTeam -c 5 -f 100")
    
    return manager

def demo_fork_intelligence():
    """Demo intelligent forking decisions"""
    print("\n🧠 DEMO: Intelligent Fork Decisions")
    print("=" * 60)
    
    from main_ai import ZappyAI
    
    ai = ZappyAI(4242, "DemoTeam", "localhost")
    
    # Simulate different game states
    scenarios = [
        {
            "name": "Early Game - Low Food",
            "turn_count": 100,
            "food": 30,
            "level": 1,
            "expected": False
        },
        {
            "name": "Early Game - Good Food",
            "turn_count": 150,
            "food": 60,
            "level": 1,
            "expected": True
        },
        {
            "name": "Mid Game - Excess Resources",
            "turn_count": 800,
            "food": 120,
            "level": 3,
            "linemate": 10,
            "expected": True
        },
        {
            "name": "High Level - Stable",
            "turn_count": 1200,
            "food": 90,
            "level": 5,
            "expected": True
        }
    ]
    
    for scenario in scenarios:
        print(f"\n📋 Scenario: {scenario['name']}")
        
        # Set AI state
        ai.state.turn_count = scenario['turn_count']
        ai.state.inventory['food'] = scenario['food']
        ai.state.level = scenario['level']
        ai.should_fork_early = True
        ai.last_fork_attempt = 0  # Reset cooldown
        
        # Add extra resources if specified
        for resource, amount in scenario.items():
            if resource not in ['name', 'turn_count', 'food', 'level', 'expected']:
                ai.state.inventory[resource] = amount
        
        # Test fork decision
        should_fork = ai.should_fork()
        expected = scenario['expected']
        
        print(f"   Turn: {scenario['turn_count']}")
        print(f"   Food: {scenario['food']}")
        print(f"   Level: {scenario['level']}")
        print(f"   Should Fork: {'✅ YES' if should_fork else '❌ NO'}")
        print(f"   Expected: {'✅ YES' if expected else '❌ NO'}")
        print(f"   Result: {'✅ CORRECT' if should_fork == expected else '❌ INCORRECT'}")

def demo_process_management():
    """Demo process management features"""
    print("\n🔧 DEMO: Process Management")
    print("=" * 60)
    
    manager = ZappyAIManager(
        port=4242,
        team_name="ProcessDemo",
        hostname="localhost",
        max_processes=2
    )
    
    print("📊 Demonstrating process management features:")
    print("   1. Spawn limit enforcement")
    print("   2. Process cleanup")
    print("   3. Statistics tracking")
    print("   4. Signal handling")
    
    # Test spawn limits
    print(f"\n🔄 Current active processes: {len(manager.active_processes)}")
    print(f"   Max processes allowed: {manager.max_processes}")
    
    # Simulate process statistics
    manager.stats['total_spawned'] = 15
    manager.stats['successful_spawns'] = 13
    manager.stats['failed_spawns'] = 2
    manager.stats['completed_ais'] = 8
    
    print("\n📊 Simulated Statistics:")
    manager.print_stats()

def main():
    """Main demo function"""
    parser = argparse.ArgumentParser(description="Demo Zappy AI Forking System")
    parser.add_argument("--demo", choices=['basic', 'intelligence', 'management', 'all'], 
                       default='all', help="Which demo to run")
    
    args = parser.parse_args()
    
    print("🚀 Zappy AI Forking System Demo")
    print("=" * 60)
    print("This demo shows the enhanced multi-process AI capabilities")
    print("that automatically spawn AIs for every available egg.")
    print()
    
    if args.demo in ['basic', 'all']:
        demo_basic_forking()
    
    if args.demo in ['intelligence', 'all']:
        demo_fork_intelligence()
    
    if args.demo in ['management', 'all']:
        demo_process_management()
    
    print("\n🎯 Demo Complete!")
    print("=" * 60)
    print("To use the enhanced AI system:")
    print("  1. Start a Zappy server:")
    print("     ./zappy_server -p 4242 -x 10 -y 10 -n MyTeam -c 5 -f 100")
    print("  2. Run the smart AI:")
    print("     python src/smart_zappy_ai.py -p 4242 -n MyTeam")
    print("  3. Watch as multiple AIs automatically spawn!")
    print()
    print("📚 For more information, see FORKING_SYSTEM.md")

if __name__ == "__main__":
    main()
