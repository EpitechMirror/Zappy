##
## NeuralNetwork.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 2:38:29 PM 2025 adrien.marette@epitech.eu
## Last update Wed Jun 10 2:38:32 PM 2025 adrien.marette@epitech.eu
##

##
## NeuralNetwork.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 1:12:56 PM 2025 adrien.marette@epitech.eu
## Last update Wed Jun 10 1:44:10 PM 2025 adrien.marette@epitech.eu
##

import os
import pickle
import numpy as np
from typing import List


class NeuralNetwork:
    """Simple feedforward neural network with backpropagation"""

    def __init__(
        self,
        input_size: int,
        hidden_sizes: List[int],
        output_size: int,
        learning_rate: float = 0.001,
    ):
        self.learning_rate = learning_rate
        self.layers = []

        # Initialize weights and biases
        layer_sizes = [input_size] + hidden_sizes + [output_size]
        for i in range(len(layer_sizes) - 1):
            weight = np.random.randn(layer_sizes[i], layer_sizes[i + 1]) * 0.1
            bias = np.zeros((1, layer_sizes[i + 1]))
            self.layers.append({"weight": weight, "bias": bias})

    def sigmoid(self, x):
        return 1 / (1 + np.exp(-np.clip(x, -500, 500)))

    def sigmoid_derivative(self, x):
        return x * (1 - x)

    def relu(self, x):
        return np.maximum(0, x)

    def relu_derivative(self, x):
        return (x > 0).astype(float)

    def forward(self, x):
        self.activations = [x]
        current_input = x

        for i, layer in enumerate(self.layers):
            z = np.dot(current_input, layer["weight"]) + layer["bias"]
            if i == len(self.layers) - 1:  # Output layer
                activation = self.sigmoid(z)
            else:  # Hidden layers
                activation = self.relu(z)

            self.activations.append(activation)
            current_input = activation

        return current_input

    def backward(self, x, y_true, y_pred):
        m = x.shape[0]

        # Calculate output error
        output_error = y_pred - y_true
        deltas = []

        # Calculate delta for output layer
        output_delta = output_error * self.sigmoid_derivative(y_pred)
        deltas.append(output_delta)

        # Backpropagate errors through hidden layers
        for i in range(len(self.layers) - 2, -1, -1):
            # Calculate delta for current layer
            error = deltas[0].dot(self.layers[i + 1]["weight"].T)
            delta = error * self.relu_derivative(self.activations[i + 1])
            deltas.insert(0, delta)

        # Update weights and biases
        for i, layer in enumerate(self.layers):
            layer["weight"] -= (
                self.learning_rate * self.activations[i].T.dot(deltas[i]) / m
            )
            layer["bias"] -= (
                self.learning_rate * np.sum(deltas[i], axis=0, keepdims=True) / m
            )

    def train(self, x, y):
        y_pred = self.forward(x)
        self.backward(x, y, y_pred)
        return np.mean((y_pred - y) ** 2)

    def predict(self, x):
        return self.forward(x)

    def save(self, filepath):
        with open(filepath, "wb") as f:
            pickle.dump({"layers": self.layers, "learning_rate": self.learning_rate}, f)

    def load(self, filepath):
        if os.path.exists(filepath):
            try:
                with open(filepath, "rb") as f:
                    data = pickle.load(f)
                    if isinstance(data, dict):
                        self.layers = data["layers"]
                        self.learning_rate = data.get(
                            "learning_rate", self.learning_rate
                        )
                    else:
                        # Handle old format
                        self.layers = data
                return True
            except Exception as e:
                print(f"Warning: Could not load model from {filepath}: {e}")
                return False
        return False
