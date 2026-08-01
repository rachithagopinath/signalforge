# signalforge

A lightweight, high-performance desktop application for real-time digital signal processing (DSP) and spectral visualization, written in C using Raylib.

## Overview

SignalForge GUI provides a real-time oscilloscope and FFT spectrum analyzer. It simulates multi-frequency audio signal generation, applies a configurable low-pass filter (LPF), and renders time-domain and frequency-domain representations at 60 FPS.

## Key Features

* Real-time Cooley-Tukey Radix-2 Fast Fourier Transform (FFT) implementation.
* Interactive Infinite Impulse Response (IIR) low-pass filter with runtime cutoff adjustment.
* Dual-panel graphical visualization:
  * Time-domain oscilloscope trace.
  * Frequency-domain magnitude spectrum bar chart.
* High-performance 60 FPS desktop rendering powered by Raylib.

## Technical Architecture

* **Language:** C (C99)
* **Graphics Library:** Raylib
* **Core Algorithms:**
  * Fast Fourier Transform (Cooley-Tukey algorithm)
  * IIR Single-Pole Low-Pass Filter
  * Synthesized Signal Generation (440 Hz base tone + high-frequency harmonics)

## Prerequisites

Ensure you have a C compiler (GCC or Clang) and the Raylib library installed on your system.
