import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

qpsk_data = pd.read_csv('./build/ber_sigma_qpsk.csv', dtype='float64')
qam16_data = pd.read_csv('./build/ber_sigma_qam16.csv', dtype='float64')
qam64_data = pd.read_csv('./build/ber_sigma_qam64.csv', dtype='float64')

plt.figure(figsize=(10, 6))

plt.plot(qpsk_data['sigma'], qpsk_data['ber'], label='QPSK', color='blue')
plt.plot(qam16_data['sigma'], qam16_data['ber'], label='QAM16', color='orange')
plt.plot(qam64_data['sigma'], qam64_data['ber'], label='QAM64', color='green')

plt.xlabel('sigma')
plt.ylabel('Bit Error Rate (BER)')
plt.title('BER vs Noise Level for QPSK, QAM16, and QAM64')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend()
plt.tight_layout()

plt.savefig('ber_comparison.png', dpi=300)

plt.show()
