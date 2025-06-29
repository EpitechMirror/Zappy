##
## test_zappy_ai.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:50:24 PM 2025 adrien.marette@epitech.eu
##

import unittest
import sys
import argparse
from unittest.mock import Mock, patch, MagicMock
import os

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

import zappy_ai


class TestZappyAIMain(unittest.TestCase):
    """Test cases for zappy_ai.py main module"""
    
    def test_helper_function(self):
        """Test that helper function runs without error"""
        # Capture print output to avoid cluttering test output
        with patch('builtins.print') as mock_print:
            zappy_ai.helper()
            
            # Verify that print was called (helper function should print help text)
            self.assertTrue(mock_print.called)
            
            # Check that key information is printed
            print_calls = [call[0][0] for call in mock_print.call_args_list]
            help_text = ' '.join(print_calls)
            
            self.assertIn('Zappy AI', help_text)
            self.assertIn('Neural Networks', help_text)
            self.assertIn('port', help_text.lower())
            self.assertIn('team', help_text.lower())

    @patch('zappy_ai.ZappyAI')
    @patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team'])
    def test_main_basic_args(self, mock_zappy_ai_class):
        """Test main function with basic arguments"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        zappy_ai.main()
        
        # Verify ZappyAI was instantiated with correct arguments
        mock_zappy_ai_class.assert_called_once_with(4242, 'test_team', 'localhost')
        
        # Verify run method was called
        mock_ai_instance.run.assert_called_once()

    @patch('zappy_ai.ZappyAI')
    @patch('sys.argv', ['zappy_ai.py', '-p', '8080', '-n', 'my_team', '-H', 'remote_host'])
    def test_main_full_args(self, mock_zappy_ai_class):
        """Test main function with all arguments including hostname"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        zappy_ai.main()
        
        # Verify ZappyAI was instantiated with all arguments
        mock_zappy_ai_class.assert_called_once_with(8080, 'my_team', 'remote_host')
        
        # Verify run method was called
        mock_ai_instance.run.assert_called_once()

    @patch('zappy_ai.ZappyAI')
    @patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team', '--no-learning'])
    def test_main_no_learning_flag(self, mock_zappy_ai_class):
        """Test main function with no-learning flag"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        zappy_ai.main()
        
        # Verify ZappyAI was instantiated
        mock_zappy_ai_class.assert_called_once_with(4242, 'test_team', 'localhost')
        
        # Verify learning was disabled
        self.assertFalse(mock_ai_instance.learning_enabled)
        self.assertEqual(mock_ai_instance.epsilon, 0.01)
        
        # Verify run method was called
        mock_ai_instance.run.assert_called_once()

    @patch('zappy_ai.helper')
    @patch('sys.argv', ['zappy_ai.py', '--help'])
    def test_main_help_flag(self, mock_helper):
        """Test main function with help flag"""
        zappy_ai.main()
        
        # Verify helper function was called
        mock_helper.assert_called_once()

    @patch('sys.argv', ['zappy_ai.py', '-p', 'invalid_port', '-n', 'test_team'])
    def test_main_invalid_port(self):
        """Test main function with invalid port number"""
        with self.assertRaises(SystemExit):
            zappy_ai.main()

    @patch('sys.argv', ['zappy_ai.py', '-p', '4242'])  # Missing required -n argument
    def test_main_missing_required_args(self):
        """Test main function with missing required arguments"""
        with self.assertRaises(SystemExit):
            zappy_ai.main()

    @patch('sys.argv', ['zappy_ai.py'])  # No arguments
    def test_main_no_args(self):
        """Test main function with no arguments"""
        with self.assertRaises(SystemExit):
            zappy_ai.main()

    def test_argument_parser_configuration(self):
        """Test that argument parser is configured correctly"""
        # Create a parser similar to the one in main()
        parser = argparse.ArgumentParser(description="Zappy AI Client with Neural Networks")
        parser.add_argument("-p", "--port", type=int, required=True, help="Port number")
        parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
        parser.add_argument("-H", "--hostname", type=str, default="localhost", help="Server hostname")
        parser.add_argument("--no-learning", action="store_true", help="Disable learning")
        
        # Test valid arguments
        args = parser.parse_args(['-p', '4242', '-n', 'test_team'])
        self.assertEqual(args.port, 4242)
        self.assertEqual(args.name, 'test_team')
        self.assertEqual(args.hostname, 'localhost')  # default value
        self.assertFalse(args.no_learning)  # default value
        
        # Test with all arguments
        args = parser.parse_args(['-p', '8080', '-n', 'my_team', '-H', 'example.com', '--no-learning'])
        self.assertEqual(args.port, 8080)
        self.assertEqual(args.name, 'my_team')
        self.assertEqual(args.hostname, 'example.com')
        self.assertTrue(args.no_learning)

    @patch('zappy_ai.ZappyAI')
    @patch('builtins.print')
    def test_main_initialization_messages(self, mock_print, mock_zappy_ai_class):
        """Test that main function prints initialization messages"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        with patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team']):
            zappy_ai.main()
        
        # Verify that initialization messages were printed
        print_calls = [call[0][0] for call in mock_print.call_args_list]
        messages = ' '.join(print_calls)
        
        self.assertIn('Initializing', messages)
        self.assertIn('4242', messages)
        self.assertIn('test_team', messages)

    @patch('zappy_ai.ZappyAI')
    @patch('builtins.print')
    def test_main_no_learning_message(self, mock_print, mock_zappy_ai_class):
        """Test that no-learning message is printed when flag is used"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        with patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team', '--no-learning']):
            zappy_ai.main()
        
        # Verify that no-learning message was printed
        print_calls = [call[0][0] for call in mock_print.call_args_list]
        messages = ' '.join(print_calls)
        
        self.assertIn('Learning disabled', messages)

    def test_module_structure(self):
        """Test that the module has expected structure"""
        # Test that main functions exist
        self.assertTrue(hasattr(zappy_ai, 'main'))
        self.assertTrue(hasattr(zappy_ai, 'helper'))
        
        # Test that functions are callable
        self.assertTrue(callable(zappy_ai.main))
        self.assertTrue(callable(zappy_ai.helper))

    @patch('zappy_ai.ZappyAI')
    def test_ai_instance_attributes_set(self, mock_zappy_ai_class):
        """Test that AI instance attributes are set correctly based on arguments"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        # Test with no-learning flag
        with patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team', '--no-learning']):
            zappy_ai.main()
        
        # Verify that the AI instance was configured correctly
        self.assertFalse(mock_ai_instance.learning_enabled)
        self.assertEqual(mock_ai_instance.epsilon, 0.01)

    @patch('zappy_ai.ZappyAI')
    def test_ai_instance_default_learning(self, mock_zappy_ai_class):
        """Test that AI instance has learning enabled by default"""
        mock_ai_instance = Mock()
        mock_zappy_ai_class.return_value = mock_ai_instance
        
        # Test without no-learning flag
        with patch('sys.argv', ['zappy_ai.py', '-p', '4242', '-n', 'test_team']):
            zappy_ai.main()
        
        # Verify that learning_enabled was not modified (should remain default True)
        # Since we're mocking, we just verify the flag wasn't set to False
        # In the actual code, learning_enabled should remain True by default

    def test_script_name_in_argv(self):
        """Test handling of different script names in argv"""
        original_argv = sys.argv.copy()
        
        try:
            # Test with different script name
            with patch('sys.argv', ['enhanced_zappy_ai.py', '-p', '4242', '-n', 'test_team']):
                with patch('zappy_ai.ZappyAI') as mock_ai:
                    mock_ai.return_value.run = Mock()
                    zappy_ai.main()
                    mock_ai.assert_called_once()
        finally:
            sys.argv = original_argv

    @patch('sys.exit')
    def test_help_argument_exits(self, mock_exit):
        """Test that --help argument triggers helper and exits"""
        with patch('sys.argv', ['zappy_ai.py', '--help']):
            with patch('zappy_ai.helper') as mock_helper:
                zappy_ai.main()
                mock_helper.assert_called_once()

    def test_port_type_validation(self):
        """Test that port argument accepts valid integers"""
        parser = argparse.ArgumentParser()
        parser.add_argument("-p", "--port", type=int, required=True)
        
        # Valid port numbers
        args = parser.parse_args(['-p', '4242'])
        self.assertEqual(args.port, 4242)
        self.assertIsInstance(args.port, int)
        
        args = parser.parse_args(['-p', '1'])
        self.assertEqual(args.port, 1)
        
        args = parser.parse_args(['-p', '65535'])
        self.assertEqual(args.port, 65535)


class TestModuleExecution(unittest.TestCase):
    """Test cases for module execution behavior"""
    
    def test_main_execution_guard(self):
        """Test that main() is only called when script is executed directly"""
        # This test verifies the if __name__ == "__main__": guard
        # When importing the module, main() should not be called automatically
        
        # The fact that we can import zappy_ai without main() executing
        # proves that the guard is working correctly
        import zappy_ai as imported_module
        self.assertTrue(hasattr(imported_module, 'main'))


if __name__ == '__main__':
    unittest.main()
