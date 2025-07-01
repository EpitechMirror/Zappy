#!/usr/bin/python3
##
## smart_zappy_ai.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Wed Jun 25 11:39:38 AM 2025 adrien.marette@epitech.eu
## Last update Mon Jun 29 3:46:51 PM 2025 adrien.marette@epitech.eu
##

import sys
import argparse
from ai_manager import ZappyAIManager

def main():
    """Enhanced Zappy AI with automatic forking and multi-process management"""
    parser = argparse.ArgumentParser(description="Smart Zappy AI - Auto-fork for maximum eggs")
    parser.add_argument("-p", "--port", type=int, required=True, help="Server port number")
    parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
    parser.add_argument("-H", "--hostname", type=str, default="localhost", help="Server hostname")
    parser.add_argument("-m", "--max-processes", type=int, default=10, help="Maximum number of AI processes")
    parser.add_argument("--single", action="store_true", help="Run single AI instead of manager")
    
    args = parser.parse_args()
    
    print("🚀 Smart Zappy AI - Maximize Team Presence")
    print("=" * 60)
    print(f"🌐 Target: {args.hostname}:{args.port}")
    print(f"👥 Team: {args.name}")
    
    if args.single:
        # Run single AI instance (backward compatibility)
        from zappy_ai import main as single_main
        sys.argv = [sys.argv[0], "-p", str(args.port), "-n", args.name, "-H", args.hostname]
        single_main()
    else:
        # Run AI manager for maximum egg utilization
        print(f"📈 Max processes: {args.max_processes}")
        print("🔄 Auto-fork mode: ENABLED")
        print("🥚 Will spawn AI for every available egg!")
        
        import multiprocessing
        multiprocessing.set_start_method('spawn', force=True)
        
        manager = ZappyAIManager(
            port=args.port,
            team_name=args.name,
            hostname=args.hostname,
            max_processes=args.max_processes
        )
        
        manager.run()

if __name__ == "__main__":
    main()
