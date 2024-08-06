import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

# Function to check if a number is prime
def is_prime(n):
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

# Function to compute the sum of primes up to a limit
def sum_of_primes(limit):
    prime_sum = 0
    primes = []
    for num in range(2, limit + 1):
        if is_prime(num):
            prime_sum += num
            primes.append(prime_sum)
    return primes

# Generating primes and their cumulative sums
limit = 1000
primes = [i for i in range(2, limit + 1) if is_prime(i)]
sum_primes = sum_of_primes(limit)

# Define an exponential function for fitting
def exponential_func(x, a, b):
    return a * np.exp(b * x)

# Convert lists to numpy arrays for compatibility
primes = np.array(primes)
sum_primes = np.array(sum_primes)

# Fit an exponential curve to the data
popt, pcov = curve_fit(exponential_func, primes, sum_primes, maxfev=10000)

# Plotting the data points and the fitted exponential curve
plt.figure(figsize=(10, 6))
plt.plot(primes, sum_primes, marker='o', linestyle='', label='Data')
plt.plot(primes, exponential_func(primes, *popt), label='Fitted Exponential Curve')
plt.title('Sum of Primes vs Primes with Fitted Exponential Curve')
plt.xlabel('Primes')
plt.ylabel('Sum of Primes')
plt.legend()
plt.grid(True)
plt.show()

# Print the parameters of the fitted exponential curve
print(f'Fitted Parameters (a, b): {popt}')
