import numpy as np
import matplotlib.pyplot as plt

# Leggi i valori dalla matrice_values.txt
matrix_values = np.loadtxt("matrix_values.txt")

# Crea il plot bidimensionale
plt.imshow(matrix_values, cmap='viridis', interpolation='nearest')
plt.colorbar()
plt.title('Distribuzione dei valori nella matrice')
plt.show()
