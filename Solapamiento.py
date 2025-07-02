import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#Damos el archivo donde se encuentran los solapamientos
nombre_solap = "solap.result1.1.txt"






#Sacamos los datos del fichero
datos = np.loadtxt(nombre_solap)
num_pasos, P = datos.shape
P = P - 1
paso = datos[:,0]
patrones = [datos[:,i+1] for i in range(P)]

#Creamos la figura
fig, ax = plt.subplots()

colormap = mpl.colormaps["tab20"]
marcadores = ['o', 's', '^', 'D', 'v', '*', 'P', 'X', 'h', '+']
minimos = [min(datos[:,i+1]) for i in range(P)]
maximos = [max(datos[:,i+1]) for i in range(P)]

if (P>=10):
    for n, patron in enumerate(patrones):
        color = colormap(n / P)
        plt.plot(paso, patron, color = color, marker='.', markersize=1, linestyle = '-')

else:
    for n, patron in enumerate(patrones):
        color = colormap(n / P)
        plt.plot(paso, patron, color = color, marker=marcadores[n], markersize=3, linestyle = '-', label = f"Solapamiento con Patron {n}")

#Delimitamos la seción visible del gráfico
plt.xlim(min(paso), max(paso))
plt.ylim(min(minimos) - 0.1, max(maximos) + 0.1)

num_etiquetas = num_pasos//10
plt.xticks(np.arange(0, num_pasos, num_etiquetas))  # De 0 a 10, paso 1

#Esto sirve para ponerle etiquietas a los ejes. Ponerlas siempre
ax.set_xlabel("Número de Pasos Monte Carlo")
ax.set_ylabel("Solapamiento (Tanto por uno)")
#Esto para añadir la leyenda al plot. En cada plot hay que poner un label para que esto funcione
if (P<10): plt.legend()

archivo = nombre_solap.removesuffix(".txt")
plt.savefig(f"{archivo}.pdf", bbox_inches = "tight")

#Si no ponemos esto, no nos va a enseñar la figura
plt.show()