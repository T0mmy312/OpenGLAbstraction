# Introduction

This project provides a low-level OpenGL abstraction implemented in modern C++, using try-catch based error handling and classes representing various OpenGL objects.

# Recommended Setup

The recommended setup is to have your main code class inherit from `egl::WindowContext` and implement your logic in the provided virtual run function.

Calling `egl::WindowContext::useContext` is recommended to avoid potential errors.

Inside of `egl::WindowContext::run` any Easy OpenGL functionality can be used safely.