#!/usr/bin/env python3
##
## run_tests.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:39:35 PM 2025 adrien.marette@epitech.eu
##

"""
Test runner for Zappy AI unit tests.
This script runs all unit tests and provides coverage reports.
"""

import os
import sys
import subprocess
import argparse


def run_tests(test_file=None, coverage=False, verbose=False):
    """
    Run unit tests with optional coverage reporting.
    
    Args:
        test_file (str): Specific test file to run (optional)
        coverage (bool): Whether to generate coverage report
        verbose (bool): Whether to run in verbose mode
    """
    
    # Change to the AI directory
    ai_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(ai_dir)
    
    # Build the pytest command
    cmd = [sys.executable, "-m", "pytest"]
    
    if verbose:
        cmd.append("-v")
    
    if coverage:
        cmd.extend(["--cov=src", "--cov-report=html", "--cov-report=term"])
    
    if test_file:
        cmd.append(f"tests/{test_file}")
    else:
        cmd.append("tests/")
    
    print(f"Running command: {' '.join(cmd)}")
    print("=" * 60)
    
    try:
        result = subprocess.run(cmd, check=False)
        return result.returncode == 0
    except FileNotFoundError:
        print("Error: pytest not found. Please install pytest:")
        print("pip install pytest pytest-cov")
        return False


def run_specific_test_class(test_file, test_class):
    """
    Run a specific test class from a test file.
    
    Args:
        test_file (str): Test file name
        test_class (str): Test class name
    """
    
    ai_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(ai_dir)
    
    cmd = [
        sys.executable, "-m", "pytest", 
        f"tests/{test_file}::{test_class}", 
        "-v"
    ]
    
    print(f"Running specific test class: {test_class}")
    print(f"Command: {' '.join(cmd)}")
    print("=" * 60)
    
    try:
        result = subprocess.run(cmd, check=False)
        return result.returncode == 0
    except FileNotFoundError:
        print("Error: pytest not found. Please install pytest:")
        print("pip install pytest")
        return False


def list_tests():
    """List all available test files and classes."""
    
    tests_dir = os.path.join(os.path.dirname(__file__), "tests")
    
    print("Available test files:")
    print("=" * 40)
    
    for file in os.listdir(tests_dir):
        if file.startswith("test_") and file.endswith(".py"):
            print(f"  📁 {file}")
            
            # Try to extract test classes
            file_path = os.path.join(tests_dir, file)
            try:
                with open(file_path, 'r') as f:
                    content = f.read()
                    
                # Simple parsing to find test classes
                lines = content.split('\n')
                for line in lines:
                    if line.strip().startswith('class Test') and 'unittest.TestCase' in line:
                        class_name = line.split('class ')[1].split('(')[0]
                        print(f"    🧪 {class_name}")
                        
            except Exception as e:
                print(f"    ❌ Error reading file: {e}")
                
            print()


def main():
    """Main function to handle command line arguments and run tests."""
    
    parser = argparse.ArgumentParser(
        description="Run Zappy AI unit tests",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_tests.py                          # Run all tests
  python run_tests.py -c                       # Run all tests with coverage
  python run_tests.py -f test_classes.py       # Run specific test file
  python run_tests.py -f test_classes.py -c TestPosition  # Run specific class
  python run_tests.py --list                   # List all available tests
        """
    )
    
    parser.add_argument(
        "-f", "--file", 
        type=str, 
        help="Specific test file to run (e.g., test_classes.py)"
    )
    
    parser.add_argument(
        "-c", "--coverage", 
        action="store_true", 
        help="Generate coverage report"
    )
    
    parser.add_argument(
        "-v", "--verbose", 
        action="store_true", 
        help="Verbose output"
    )
    
    parser.add_argument(
        "--class", 
        dest="test_class",
        type=str, 
        help="Specific test class to run (requires -f)"
    )
    
    parser.add_argument(
        "--list", 
        action="store_true", 
        help="List all available test files and classes"
    )
    
    args = parser.parse_args()
    
    if args.list:
        list_tests()
        return
    
    if args.test_class and not args.file:
        print("Error: --class requires -f (test file) to be specified")
        return
    
    print("🤖 Zappy AI Test Runner")
    print("=" * 60)
    
    if args.test_class:
        success = run_specific_test_class(args.file, args.test_class)
    else:
        success = run_tests(args.file, args.coverage, args.verbose)
    
    if success:
        print("\n✅ All tests passed!")
    else:
        print("\n❌ Some tests failed!")
        sys.exit(1)


if __name__ == "__main__":
    main()
