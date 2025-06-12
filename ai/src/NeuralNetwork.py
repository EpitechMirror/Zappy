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
    """Simple feedforward neural network with backprogramation"""
    
    def __init__(self, input_size: int, hidden_sizes: List[int], output_size: int, learning_rate: float = 0.001):
        self.learning_rate = learning_rate
        self.layers = []
        
        # Initialize weights and biases
        layer_sizes = [input_size] + hidden_sizes + [output_size]
        for i in range(len(layer_sizes) - 1):
            weight = np.random.randn(layer_sizes[i], layer_sizes[i + 1]) * 0.1
            bias = np.zeros((1, layer_sizes[i + 1]))
            self.layers.append({'weight': weight, 'bias': bias})

    def sigmoid(self, x):
        return 1 / (1 + np.exp(-np.clip(x, -500, 500)))
    
    def sigmoid_relative(self, x):
        return x * (1 - x)
    
    def relu(self, x):
        return np.maximum(0, x)
    
    def relu_derivative(self, x):
        return (x > 0).astype(float)
    
    def forward(self, x):
        self.activations = [x]
        current_input = x
    
        for i, layer in enumerate(self.layers):
            z = np.dot(current_input, layer['weight']) + layer['bias']
            if i == len(self.layers) - 1: # Output layer
                activation = self.sigmoid(z)
            else: # Hidden layers
                activation = self.relu(z)

            self.activations.append(activation)
            current_input = activation

        return current_input

    def backward(self, x, y_true, y_pred):
        m = x.shape[0]

        # Calculate output error
        output_error = y_pred - y_true
        deltas = [output_error]

        # Backpropagate errors
        for i in range(len(self.layers) - 1, 0, -1):
            if i == len(self.layers) - 1: 
                delta = output_error * self.sigmoid_derivative(self.activations[i + 1])
            else:
                delta = deltas[0].dot(self.layers[i + 1]['weigth'].T) * self.relu_derivative(self.activations[i + 1])
            deltas.insert(0, delta)

        # Update weights and biases
        for i, layer in enumerate(self.layers):
            layer['weight'] -= self.learning_rate * self.activations[i].T.dot(deltas[i]) / m
            layer['bias'] -= self.learning_rate * np.sum(deltas[i], axis=0, keepdims=True) / m
            
    def train(self, x, y):
        y_pred = self.forward(x)
        self.backward(x, y, y_pred)
        return np.mean((y_pred - y) ** 2)

    def predict(self, x):
        return self.forward(x)
    
    def save(self, filepath):
        with open(filepath, 'wb') as f:
            pickle.dump(self.layers, f)

    def load(self, filepath):
        if os.path.exists(filepath):
            with open(filepath, 'rb') as f:
                self.layers = pickle.load(f)
            return True
        return False