#pragma once

#define INTERSECTS(a, b) ((a.x > b.x) && (a.x < b.x + b.width) && (a.y > b.y) && (a.y < b.y + b.height))

#define ROUND4(num) (round((num) * 10000.0f)/10000.0f)

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