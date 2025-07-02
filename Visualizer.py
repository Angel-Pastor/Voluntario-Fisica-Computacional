import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation

N = 30                              #Para ejercicios 1, 2 y 3 = 30, Para ejercicio 4 = 20
nombre_results = "result1.1.txt"    #Nombre del archivo donde se almacenaron los estados del sistema
fps = 5                             #Numero de frames por segundo






#Sacamos los valores de los estados del fichero que escribamos
s = np.loadtxt(nombre_results)

#Buscamos la dimensión de s referida a los pasos Monte_Carlo
pasos = s.size//(N*N)

#Este comando transforma el array a uno de " " bloques, 5 filas y 5 columnas
s = s.reshape(pasos,N,N)

#Preparamos la animación
fig, ax = plt.subplots()
ax.set_xticks([])
ax.set_yticks([])

im = ax.imshow(s[0], cmap='jet', vmin=0, vmax=1)

frame_text = ax.text(0.05, 0.95, '', transform=ax.transAxes, fontsize=9, color='white', verticalalignment='top')

saltador = 1                        #Cantidad de Pasos Monte Carlo que se salta de un frame a otro en el gif

#Definimos la función de la animación
def actualizar(frame):
    frame = frame
    im.set_array(s[frame])
    frame_text.set_text(f'Frame: {frame+1}/{len(s)//saltador}   Paso Monte Carlo: {saltador*frame}')
    return [im, frame_text]

#Ejecutamos la animación
fps = fps/1000
ani = animation.FuncAnimation(fig, actualizar, frames=len(s)//saltador, interval=1/fps, blit=True)

plt.show()

#Para guardar el gif que muestra los estados:
#ani.save(filename = "sans.gif", writer = "pillow")