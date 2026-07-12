#pragma once

#include <array>
// #include <algorithm>
// #include <cstddef>

#define INTERSECTS(a, b) ((a.x > b.x) && (a.x < b.x + b.width) && (a.y > b.y) && (a.y < b.y + b.height))

#define ROUND4(num) (round((num) * 10000.0f)/10000.0f)

// #define INVERSE_EXPONENTIAL(x) ((x) >= 1 ? 1 : 1 - powf(2, -10 * (x)))

static inline int factorial(int n) {
    if (n < 0) return 0;
    
    int result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }

    return result;
}

static inline int sumCount(int n) {
	return (n*(n+1))/2;
}

// #define MAX_FRAMES 30
// // #define EASE_OUT(frame) INVERSE_EXPONENTIAL((1.0f / (frame)) * MAX_FRAMES)

// static inline constexpr int inverseExp(int n) {
//     if (n >= 1.0f) {
//         return 1.0f;
//     } else {
//         return 1.0f - powf(2, -10*n);
//     }
// }

// constexpr std::array<int, MAX_FRAMES+1> generate_frames() {
//     std::array<int, MAX_FRAMES+1> frames{};
//     for (int i = MAX_FRAMES; i > 0; --i) {
//         frames[i] = 1/inverseExp(i)*MAX_FRAMES;
//     }
//     return frames;
// }

// constexpr const auto ANIM_FRAMES = generate_frames();

// // A simple constexpr power function for 2^x 
// // since std::pow is not constexpr
// constexpr float constexpr_pow2(float exp) {
//     if (exp == 0) return 1.0f;
//     if (exp < 0) return 1.0f / constexpr_pow2(-exp);
    
//     float res = 1.0f;
//     // This is a simplified version; for non-integers, 
//     // wed need a more complex Taylor series, but for 
//     // easing, we can usually work with normalized steps.
//     // However, for the sake of the demo, we'll use a loop 
//     // if the exponent is an integer.
//     for (int i = 0; i < (int)exp; ++i) res *= 2.0f;
//     return res;
// }

// // Exponential Ease In: slow start, fast end
// // t is normalized time (0.0 to 1.0)
// constexpr float easeInExpo(float t) {
//     if (t <= 0.0f) return 0.0f;
//     if (t >= 1.0f) return 1.0f;
//     // Standard formula: 2^(10 * (t-1))
//     // We use a manual calculation for constexpr compatibility
//     float power = 10.0f * (t - 1.0f);
    
//     // Since pow is not constexpr, we use a approximation or 
//     // handle it via a pre-computed table. 
//     // For simplicity in this example, I'll provide a logic 
//     // that allows the function to be constexpr.
//     return std::pow(2.0f, power); // Note: See below regarding constexpr
// }

// // To make this TRULY constexpr, we can't use std::pow.
// // Instead, we'll use a helper that simulates the curve.
// constexpr float fast_expo(float t) {
//     // Approximating 2^(10(t-1)) using a polynomial or simplified loop
//     // For a real system, you'd use a pre-computed LUT or a 
//     // non-constexpr generator.
//     float v = t;
//     v *= v; v *= v; v *= v; v *= v; // t^5 is a decent "ease in" approximation
//     return v;
// }

// // Ease-Out Approximation: Fast start, slow end
// // This is the constexpr equivalent of: 1 - 2^(-10t)
// constexpr float fast_easeOut(float t) {
//     // We calculate (1-t)^5
//     float invT = 1.0f - t;
//     float invT2 = invT * invT;
//     float invT5 = invT2 * invT2 * invT;
    
//     // Subtract from 1 to flip the curve
//     return 1.0f - invT5;
// }

// template<size_t MaxFrames>
// constexpr std::array<int, MaxFrames + 1> generate_frames_ease_out() {
//     std::array<int, MaxFrames + 1> frames{};
    
//     for (size_t i = 0; i <= MaxFrames; ++i) {
//         // 1. Normalize time (0.0 to 1.0)
//         float t = static_cast<float>(i) / static_cast<float>(MaxFrames);
        
//         // 2. Apply Ease-Out curve
//         float eased = fast_easeOut(t);
        
//         // 3. Map to frame range [1, MaxFrames]
//         // We use (MaxFrames - 1) and add 1 to ensure we stay 
//         // strictly within the 1 to 30 range.
//         int frame = 1 + static_cast<int>(eased * (MaxFrames - 1));
        
//         frames[i] = frame;
//     }
//     return frames;
// }

// // --- Usage ---
// static constexpr int MAX_FRAMES = 60;
// static constexpr auto ANIM_FRAMES = generate_frames_ease_out<MAX_FRAMES>();

// template<size_t MaxFrames>
// constexpr std::array<int, MaxFrames + 1> generate_frames() {
//     std::array<int, MaxFrames + 1> frames{};
//     for (size_t i = 0; i <= MaxFrames; ++i) {
//         // 1. Normalize time to 0.0 -> 1.0
//         float t = static_cast<float>(i) / static_cast<float>(MaxFrames);
        
//         // 2. Apply easing (t^5 is a common substitute for Expo in constexpr)
//         float eased = fast_expo(t);
        
//         // 3. Scale back to frame range and clamp to minimum 1
//         int frame = static_cast<int>(eased * MaxFrames);
//         frames[i] = (frame < 1) ? 1 : frame;
//     }
//     return frames;
// }

// // Usage
// static constexpr int MAX_FRAMES = 30;
// static constexpr auto ANIM_FRAMES = generate_frames<MAX_FRAMES>();


// JS example of getting ease out functions
// function easeOutCubic(x) {
// return 1 - Math.pow(1 - x, 3);
// }
// undefined
// var c  = [];
// undefined
// for (let i = 1; i < 43; i++) { c.push(1/easeOutCubic(i)) }
// 42
// c.reduce((mem, val) => { return `${mem} ${val.toPrecision(2)}f`; })

static constexpr std::array<float, 60> ANIM_EASE_IN_OUT_60 = { 0.000019f, 0.00015f, 0.00050f, 0.0012f, 0.0023f, 0.0040f, 0.0064f, 0.0095f, 0.013f, 0.019f, 0.025f, 0.032f, 0.041f, 0.051f, 0.063f, 0.076f, 0.091f, 0.11f, 0.13f, 0.15f, 0.17f, 0.20f, 0.23f, 0.26f, 0.29f, 0.33f, 0.36f, 0.41f, 0.45f, 0.50f, 0.55f, 0.59f, 0.64f, 0.67f, 0.71f, 0.74f, 0.77f, 0.80f, 0.83f, 0.85f, 0.87f, 0.89f, 0.91f, 0.92f, 0.94f, 0.95f, 0.96f, 0.97f, 0.98f, 0.98f, 0.99f, 0.99f, 0.99f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

static constexpr std::array<float, 42> ANIM_EASE_OUT_QUAD = { 1.0f, 0.50f, 0.11f, 0.036f, 0.015f, 0.0079f, 0.0046f, 0.0029f, 0.0019f, 0.0014f, 0.0010f, 0.00075f, 0.00058f, 0.00045f, 0.00036f, 0.00030f, 0.00024f, 0.00020f, 0.00017f, 0.00015f, 0.00012f, 0.00011f, 0.000094f, 0.000082f, 0.000072f, 0.000064f, 0.000057f, 0.000051f, 0.000046f, 0.000041f, 0.000037f, 0.000034f, 0.000031f, 0.000028f, 0.000025f, 0.000023f, 0.000021f, 0.000020f, 0.000018f, 0.000017f, 0.000016f, 0.000015f };
static constexpr std::array<float, 42> ANIM_FRAMES = { 0.000054f, 0.00043f, 0.0015f, 0.0035f, 0.0067f, 0.012f, 0.019f, 0.028f, 0.039f, 0.054f, 0.072f, 0.093f, 0.12f, 0.15f, 0.18f, 0.22f, 0.27f, 0.31f, 0.37f, 0.43f, 0.50f, 0.57f, 0.63f, 0.69f, 0.73f, 0.78f, 0.82f, 0.85f, 0.88f, 0.91f, 0.93f, 0.95f, 0.96f, 0.97f, 0.98f, 0.99f, 0.99f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
