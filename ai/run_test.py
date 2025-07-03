#!/usr/bin/env python3
##
## quick_test.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jul 4 01:20:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 01:20:00 AM 2025 adrien.marette@epitech.eu
##

"""
Quick test runner that patches slow operations for faster testing.
"""

import unittest
import sys
import os
from unittest.mock import patch

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def patch_slow_operations():
    """Patch slow operations to make tests run faster."""
    
    # Patch Team.wait_for_response to return immediately
    def fast_wait_for_response(self, timeout=2.0):
        pass
    
    # Patch time.sleep to do nothing
    def fast_sleep(duration):
        pass
    
    # Apply patches
    import Team
    import time
    Team.Team.wait_for_response = fast_wait_for_response
    time.sleep = fast_sleep

def run_quick_tests():
    """Run tests with performance patches."""
    
    print("Quick Test Runner - Performance Optimized")
    print("=" * 50)
    
    # Apply patches
    patch_slow_operations()
    
    # Test modules to run
    test_modules = [
        'test_team_base',
        'test_team_collector', 
        'test_team_roles'
    ]
    
    # Create test suite
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Load tests from each module
    for module_name in test_modules:
        try:
            module = __import__(module_name)
            module_suite = loader.loadTestsFromModule(module)
            suite.addTest(module_suite)
            print(f"✓ Loaded tests from {module_name}")
        except ImportError as e:
            print(f"✗ Failed to load {module_name}: {e}")
            
    print()
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=1)
    result = runner.run(suite)
    
    # Summary
    print("\n" + "=" * 50)
    print("QUICK TEST SUMMARY")
    print("=" * 50)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print("\nFAILURES:")
        for test, traceback in result.failures:
            print(f"- {test}")
            
    if result.errors:
        print("\nERRORS:")
        for test, traceback in result.errors:
            print(f"- {test}")
    
    success_rate = ((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100) if result.testsRun > 0 else 0
    print(f"\nSuccess rate: {success_rate:.1f}%")
    
    if success_rate == 100.0:
        print("🎉 All tests passed!")
    elif success_rate >= 90.0:
        print("✅ Most tests passed!")
    else:
        print("⚠️  Some tests failed")
        
    return result.wasSuccessful()

if __name__ == '__main__':
    success = run_quick_tests()
    sys.exit(0 if success else 1)
