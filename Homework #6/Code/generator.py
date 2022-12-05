import numpy as np
import sys

a_1, a_2, a_3 = 1.5, 2, -7
a_4, a_5, a_6 = 3.14, 0, 1.2345

if __name__ == '__main__':
    n = 10000
    noise_level = 1
    if len(sys.argv) >= 2:
        n = int(sys.argv[1])
    if len(sys.argv) >= 3:
        noise_level = int(sys.argv[2])

    # Generate n random numbers between 0 and 1
    noise = np.random.normal(0, noise_level, n)
    scale = [1, 2]
    
    x = np.random.rand(n) * scale[0]
    y = np.random.rand(n) * scale[1]
    x_prime = a_1 * x + a_2 * y + a_3 + noise
    y_prime = a_4 * x + a_5 * y + a_6 + noise
    
    for item in zip(x,y,x_prime,y_prime):
        print(*item)
    