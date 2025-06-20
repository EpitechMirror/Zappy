#!/usr/bin/python3
##
## ai.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 12:46:14 PM 2025 adrien.marette@epitech.eu
## Last update Fri Jun 12 3:21:11 PM 2025 adrien.marette@epitech.eu
##

import sys
import argparse
from ai import ZappyAI


def helper():
    print("🤖 Enhanced Zappy AI with Neural Networks and Learning")
    print("=" * 60)
    print("Usage: enhanced_zappy_ai.py -p <port> -n <team_name> [-H <hostname>]")
    print("\nOptions:")
    print("  -p, --port       Port number to connect to the Zappy server")
    print("  -n, --name       Team name for the Zappy AI")
    print("  -H, --hostname   Hostname of the Zappy server (default: localhost)")
    print("  --help           Show this help message and exit")
    print("\nFeatures:")
    print("  🧠 Neural Network Decision Making")
    print("  📚 Experience-based Learning")
    print("  💭 Advanced Thinking and Reasoning")
    print("  📊 Performance Tracking and Metrics")
    print("  🔄 Adaptive Learning Parameters")
    print("  💾 Model Persistence (saves/loads automatically)")
    print("  🎯 Strategic Planning and Goal Management")


def main():
    parser = argparse.ArgumentParser(description="Zappy AI Client with Neural Networks")
    parser.add_argument("-p", "--port", type=int, required=True, help="Port number")
    parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
    parser.add_argument(
        "-H", "--hostname", type=str, default="localhost", help="Server hostname"
    )
    parser.add_argument(
        "--no-learning",
        action="store_true",
        help="Disable learning (use existing model only)",
    )

    if any(arg in ["--help"] for arg in sys.argv):
        helper()
        return

    args = parser.parse_args()

    print("🎮 Initializing Zappy AI...")
    print(f"🌐 Connecting to {args.hostname}:{args.port}")
    print(f"👥 Team: {args.name}")

    ai = ZappyAI(args.port, args.name, args.hostname)

    if args.no_learning:
        ai.learning_enabled = False
        ai.epsilon = 0.01  # Minimal exploration
        print("🔒 Learning disabled - using existing model only")

    ai.run()


if __name__ == "__main__":
    main()
