import matplotlib.pyplot as plt

def is_prime(n):
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

def sum_of_primes(limit):
    prime_sum = 0
    primes = []
    for num in range(2, limit + 1):
        if is_prime(num):
            prime_sum += num
            primes.append(prime_sum)
    return primes

limit = 1000
primes = [i for i in range(2, limit + 1) if is_prime(i)]
sum_primes = sum_of_primes(limit)

plt.figure(figsize=(10, 6))
plt.plot(primes, sum_primes, marker='o', linestyle='-', color='b')
plt.title('Sum of Primes vs Primes')
plt.xlabel('Primes')
plt.ylabel('Sum of Primes')
plt.grid(True)
plt.show()