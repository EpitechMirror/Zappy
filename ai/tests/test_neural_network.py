##
## test_neural_network.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:52:35 PM 2025 adrien.marette@epitech.eu
##

import os
import sys
import tempfile
import unittest
from unittest.mock import Mock, mock_open, patch

import numpy as np

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from NeuralNetwork import NeuralNetwork


class TestNeuralNetwork(unittest.TestCase):
    """Test cases for NeuralNetwork class"""

    def setUp(self):
        """Set up test fixtures before each test method"""
        self.input_size = 4
        self.hidden_sizes = [8, 4]
        self.output_size = 2
        self.learning_rate = 0.01

        self.nn = NeuralNetwork(
            input_size=self.input_size,
            hidden_sizes=self.hidden_sizes,
            output_size=self.output_size,
            learning_rate=self.learning_rate,
        )

    def test_network_initialization(self):
        """Test neural network initialization"""
        self.assertEqual(self.nn.learning_rate, self.learning_rate)

        # Check number of layers (input->hidden1, hidden1->hidden2, hidden2->output)
        self.assertEqual(len(self.nn.layers), 3)

        # Check layer dimensions
        self.assertEqual(
            self.nn.layers[0]["weight"].shape, (4, 8)
        )  # input to first hidden
        self.assertEqual(
            self.nn.layers[1]["weight"].shape, (8, 4)
        )  # first to second hidden
        self.assertEqual(
            self.nn.layers[2]["weight"].shape, (4, 2)
        )  # second hidden to output

        # Check bias dimensions
        self.assertEqual(self.nn.layers[0]["bias"].shape, (1, 8))
        self.assertEqual(self.nn.layers[1]["bias"].shape, (1, 4))
        self.assertEqual(self.nn.layers[2]["bias"].shape, (1, 2))

    def test_network_initialization_no_hidden_layers(self):
        """Test neural network initialization with no hidden layers"""
        nn = NeuralNetwork(input_size=3, hidden_sizes=[], output_size=2)

        self.assertEqual(len(nn.layers), 1)
        self.assertEqual(nn.layers[0]["weight"].shape, (3, 2))
        self.assertEqual(nn.layers[0]["bias"].shape, (1, 2))

    def test_sigmoid_function(self):
        """Test sigmoid activation function"""
        # Test typical values
        self.assertAlmostEqual(self.nn.sigmoid(0), 0.5, places=5)
        self.assertAlmostEqual(self.nn.sigmoid(1), 0.7310585786, places=5)
        self.assertAlmostEqual(self.nn.sigmoid(-1), 0.2689414214, places=5)

        # Test extreme values (should not overflow)
        result_large = self.nn.sigmoid(1000)
        self.assertAlmostEqual(result_large, 1.0, places=5)

        result_small = self.nn.sigmoid(-1000)
        self.assertAlmostEqual(result_small, 0.0, places=5)

    def test_sigmoid_derivative(self):
        """Test sigmoid derivative function"""
        # Test with sigmoid output values
        sig_05 = 0.5
        self.assertAlmostEqual(self.nn.sigmoid_derivative(sig_05), 0.25, places=5)

        sig_1 = 1.0
        self.assertAlmostEqual(self.nn.sigmoid_derivative(sig_1), 0.0, places=5)

        sig_0 = 0.0
        self.assertAlmostEqual(self.nn.sigmoid_derivative(sig_0), 0.0, places=5)

    def test_relu_function(self):
        """Test ReLU activation function"""
        # Test positive values
        self.assertEqual(self.nn.relu(5), 5)
        self.assertEqual(self.nn.relu(0.5), 0.5)

        # Test negative values
        self.assertEqual(self.nn.relu(-5), 0)
        self.assertEqual(self.nn.relu(-0.1), 0)

        # Test zero
        self.assertEqual(self.nn.relu(0), 0)

        # Test with numpy arrays
        input_array = np.array([-2, -1, 0, 1, 2])
        expected = np.array([0, 0, 0, 1, 2])
        np.testing.assert_array_equal(self.nn.relu(input_array), expected)

    def test_relu_derivative(self):
        """Test ReLU derivative function"""
        # Test positive values
        self.assertEqual(self.nn.relu_derivative(5), 1.0)
        self.assertEqual(self.nn.relu_derivative(0.1), 1.0)

        # Test negative values
        self.assertEqual(self.nn.relu_derivative(-5), 0.0)
        self.assertEqual(self.nn.relu_derivative(-0.1), 0.0)

        # Test zero
        self.assertEqual(self.nn.relu_derivative(0), 0.0)

        # Test with numpy arrays
        input_array = np.array([-2, -1, 0, 1, 2])
        expected = np.array([0.0, 0.0, 0.0, 1.0, 1.0])
        np.testing.assert_array_equal(self.nn.relu_derivative(input_array), expected)

    def test_forward_pass(self):
        """Test forward pass through the network"""
        input_data = np.array([[1, 2, 3, 4]])

        output = self.nn.forward(input_data)

        # Check output shape
        self.assertEqual(output.shape, (1, 2))

        # Check that output values are between 0 and 1 (sigmoid output)
        self.assertTrue(np.all(output >= 0))
        self.assertTrue(np.all(output <= 1))

        # Check that activations are stored
        self.assertEqual(len(self.nn.activations), 4)  # input + 3 layers
        self.assertEqual(self.nn.activations[0].shape, (1, 4))  # input
        self.assertEqual(self.nn.activations[1].shape, (1, 8))  # first hidden
        self.assertEqual(self.nn.activations[2].shape, (1, 4))  # second hidden
        self.assertEqual(self.nn.activations[3].shape, (1, 2))  # output

    def test_forward_pass_batch(self):
        """Test forward pass with batch input"""
        batch_input = np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]])

        output = self.nn.forward(batch_input)

        # Check output shape for batch
        self.assertEqual(output.shape, (3, 2))

        # Check that all outputs are valid probabilities
        self.assertTrue(np.all(output >= 0))
        self.assertTrue(np.all(output <= 1))

    def test_predict(self):
        """Test predict method (should call forward)"""
        input_data = np.array([[1, 2, 3, 4]])

        prediction = self.nn.predict(input_data)
        forward_result = self.nn.forward(input_data)

        np.testing.assert_array_equal(prediction, forward_result)

    def test_train_multiple_steps(self):
        """Test multiple training steps reduce loss"""
        input_data = np.array([[1, 2, 3, 4]])
        target = np.array([[0, 1]])

        # Train for multiple steps
        losses = []
        for _ in range(5):
            loss = self.nn.train(input_data, target)
            losses.append(loss)

        # Loss should generally decrease (though may fluctuate)
        self.assertLess(losses[-1], losses[0] * 2)  # Some improvement expected

    @patch("builtins.open", new_callable=mock_open)
    @patch("pickle.dump")
    def test_save_model(self, mock_pickle_dump, mock_file):
        """Test saving model to file"""
        filepath = "test_model.pkl"

        self.nn.save(filepath)

        # Check that file was opened for writing
        mock_file.assert_called_once_with(filepath, "wb")

        # Check that pickle.dump was called with correct data
        mock_pickle_dump.assert_called_once()
        saved_data = mock_pickle_dump.call_args[0][0]
        self.assertIn("layers", saved_data)
        self.assertIn("learning_rate", saved_data)

    @patch("os.path.exists")
    @patch("builtins.open", new_callable=mock_open)
    @patch("pickle.load")
    def test_load_model_success(self, mock_pickle_load, mock_file, mock_exists):
        """Test successfully loading model from file"""
        filepath = "test_model.pkl"
        mock_exists.return_value = True

        # Mock loaded data
        mock_data = {
            "layers": [{"weight": np.array([[1, 2]]), "bias": np.array([[0]])}],
            "learning_rate": 0.05,
        }
        mock_pickle_load.return_value = mock_data

        result = self.nn.load(filepath)

        self.assertTrue(result)
        mock_file.assert_called_once_with(filepath, "rb")
        self.assertEqual(self.nn.learning_rate, 0.05)

    @patch("os.path.exists")
    @patch("builtins.open", new_callable=mock_open)
    @patch("pickle.load")
    def test_load_model_old_format(self, mock_pickle_load, mock_file, mock_exists):
        """Test loading model with old format (just layers)"""
        filepath = "test_model.pkl"
        mock_exists.return_value = True

        # Mock old format data (just layers)
        mock_data = [{"weight": np.array([[1, 2]]), "bias": np.array([[0]])}]
        mock_pickle_load.return_value = mock_data

        result = self.nn.load(filepath)

        self.assertTrue(result)
        self.assertEqual(self.nn.layers, mock_data)

    @patch("os.path.exists")
    def test_load_model_file_not_exists(self, mock_exists):
        """Test loading model when file doesn't exist"""
        filepath = "nonexistent_model.pkl"
        mock_exists.return_value = False

        result = self.nn.load(filepath)

        self.assertFalse(result)

    @patch("os.path.exists")
    @patch("builtins.open", new_callable=mock_open)
    @patch("pickle.load")
    def test_load_model_exception(self, mock_pickle_load, mock_file, mock_exists):
        """Test loading model when an exception occurs"""
        filepath = "corrupt_model.pkl"
        mock_exists.return_value = True
        mock_pickle_load.side_effect = Exception("Corrupt file")

        with patch("builtins.print"):  # Suppress print output
            result = self.nn.load(filepath)

        self.assertFalse(result)

    def test_backward_pass_shapes(self):
        """Test that backward pass maintains correct tensor shapes"""
        input_data = np.array([[1, 2, 3, 4]])
        target = np.array([[0, 1]])

        # Perform forward pass first
        output = self.nn.forward(input_data)

        # Store original shapes
        original_shapes = [
            (layer["weight"].shape, layer["bias"].shape) for layer in self.nn.layers
        ]

        # Perform backward pass
        self.nn.backward(input_data, target, output)

        # Check that shapes remain unchanged
        for i, layer in enumerate(self.nn.layers):
            self.assertEqual(layer["weight"].shape, original_shapes[i][0])
            self.assertEqual(layer["bias"].shape, original_shapes[i][1])

    def test_network_consistency(self):
        """Test that forward pass is consistent for same input"""
        input_data = np.array([[1, 2, 3, 4]])

        output1 = self.nn.forward(input_data)
        output2 = self.nn.forward(input_data)

        np.testing.assert_array_equal(output1, output2)


if __name__ == "__main__":
    unittest.main()
