import numpy as np
import matplotlib.pyplot as plt

# Example dataset
throughputs1 = np.array([20.4324, 20.4324, 22.3271, 20.4324, 21.3841])
throughputs2 = np.array([21.3841, 23.2266, 20.4324, 22.3271, 21.4559])

# Sort the dataset
sorted_throughputs1 = np.sort(throughputs1)
sorted_throughputs2 = np.sort(throughputs2)

# Calculate the cumulative probability
num_values1 = len(throughputs1)
num_values2 = len(throughputs2)

cumulative_probabilities1 = np.arange(1, num_values1 + 1) / num_values1
cumulative_probabilities2 = np.arange(1, num_values2 + 1) / num_values2

# Plot the CDF
plt.figure()
# plt.plot(sorted_throughputs1, cumulative_probabilities1, marker='o', label='MT Throughput 0m/s')
# plt.plot(sorted_throughputs2, cumulative_probabilities2, marker='p', label='MT Throughput 10m/s')
# plt.plot(sorted_throughputs1, cumulative_probabilities1, marker='o', label='PF Throughput 0m/s')
# plt.plot(sorted_throughputs2, cumulative_probabilities2, marker='p', label='PF Throughput 10m/s')
plt.plot(sorted_throughputs1, cumulative_probabilities1, marker='o', label='PSS Throughput 0m/s')
plt.plot(sorted_throughputs2, cumulative_probabilities2, marker='p', label='PSS Throughput 10m/s')
# plt.plot(sorted_throughputs1, cumulative_probabilities1, marker='o', label='RR Throughput 0m/s')
# plt.plot(sorted_throughputs2, cumulative_probabilities2, marker='p', label='RR Throughput 10m/s')
plt.xlabel('Throughput (Mbps)')
plt.ylabel('Cumulative Probability')
plt.title('Cumulative Distribution Function (CDF) of Throughput')

# Add throughput values as text
for x1, y1, throughput1 in zip(sorted_throughputs1, cumulative_probabilities1, sorted_throughputs1):
    plt.text(x1, y1, f'{throughput1:.4f}', verticalalignment='bottom', horizontalalignment='right')

for x2, y2, throughput2 in zip(sorted_throughputs2, cumulative_probabilities2, sorted_throughputs2):
    plt.text(x2, y2, f'{throughput2:.4f}', verticalalignment='bottom', horizontalalignment='right')

plt.legend()
plt.grid(True)
plt.show()