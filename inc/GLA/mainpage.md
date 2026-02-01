# Introduction

This project provides a low-level OpenGL abstraction implemented in modern C++, using try-catch based error handling and classes representing various OpenGL objects.

# Recommended Setup

The recommended setup is to have your main code class inherit from `gla::WindowContext` and implement your logic in the provided virtual run function.

Calling `gla::WindowContext::useContext` is recommended to avoid potential errors.

Inside of `gla::WindowContext::run` any OpenGL Abstraction functionality can be used safely.