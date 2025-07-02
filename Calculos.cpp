#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <cmath>
#include <fstream>

/*
Al usar config1.txt, N = 30, P_max =   1 (Ejercicio 1 y 2: Rana)
Al usar config2.txt, N = 30, P_max =   3 (Ejercicio 3: Rectángulos Verticales)
Al usar config3.txt, N = 20, P_max = 100 (Ejercicio 4: Patrones Aleatorios)
*/

#define reps 1
#define inicial -1
#define T 0.0001
#define Monte_Carlo 20 //Mínimo 5
#define N 30
#define P 1

using namespace std;
double omega(short int n, short int m, short int k, short int l, short int epsilon[][N][P], double a[]);
void calculo_solapamiento(double solapamiento[], short int s[][N], short int epsilon[][N][P], double a[], short int mu);

int main (void)
{
    string nombre_inicial = "predeterminado.txt"; //Donde se tiene o donde se guardará la configuración inicial de la red.
    string nombre_configs = "config1.txt"; //Donde se tienen almacenadas las configuraciones guardadas de la red.
    string nombre_results = "result1.1.txt"; //Donde se almacena el estado del sistema tras cada paso Monte Carlo.

    string ruta_fichero_sistema_inicial = "C:\\Users\\Usuario\\Documents\\1UNIVERSIDAD\\Curso3Cuatri2\\Computacional\\1VOLUNTARIOS\\iniciales\\";
    string ruta_fichero_config_guardadas = "C:\\Users\\Usuario\\Documents\\1UNIVERSIDAD\\Curso3Cuatri2\\Computacional\\1VOLUNTARIOS\\configs\\";
    string ruta_fichero_resultados = "C:\\Users\\Usuario\\Documents\\1UNIVERSIDAD\\Curso3Cuatri2\\Computacional\\1VOLUNTARIOS\\";
    





    string ruta_fichero_solapamientos;
    ruta_fichero_solapamientos = ruta_fichero_resultados + "solap." + nombre_results;
    ruta_fichero_sistema_inicial = ruta_fichero_sistema_inicial + nombre_inicial;
    ruta_fichero_config_guardadas = ruta_fichero_config_guardadas + nombre_configs;
    ruta_fichero_resultados = ruta_fichero_resultados + nombre_results;
    
    
    //Sección del programa para asegurar que hemos cambiado el fichero de salida y hemos seleccionado las Configuraciones Guardadas correctamente 
    string confirmacion;
    cout << "¿Has cambiado los nombres de los ficheros involucrados para no sobreescribir? Y/N: ";
    cin >> confirmacion;

    if ((confirmacion == "n")||(confirmacion == "N"))
    {
        return 0;
    }
    
    ifstream config_guardadas(ruta_fichero_config_guardadas);
    
    if (!config_guardadas)
    {
        cout << "No se pudo abrir el fichero de configuraciones almacenadas.";
        return 0;
    }

    //Preparamos el ficheros de salida que usaremos para entregarle los datos al programa visualizador de python
    ofstream fichero_estados(ruta_fichero_resultados);
    //Preparamos también el fichero donde almacenamos los pasos monte carlo y los solapamientos correspondientes a cada patrón
    ofstream fichero_solapamientos(ruta_fichero_solapamientos);
    fichero_solapamientos <<  "0";

    //En cada iteración elegiremos aleatoriamente un nodo al que evaluar y una probabilidad con la que comparar la nuestra
    //Preparamos las distribuciones aleatorias de enteros y reales.
    unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
    mt19937_64 generator(seed1);
    uniform_int_distribution<int> i_distribution(0, N-1);
    uniform_real_distribution<double> r_distribution(0., 1.);

    //s será mi sistema de N*N neuronas. n, m, k y l correrán las filas y columnas.
    //n_cambio y m_cambio almacenan la posición del nodo que se ha elegido aleatoriamente para cambiar su estado.
    //i correrá cada paso Monte Carlo.
    //j correrá las repeticiones dentro de cada paso Monte Carlo para que cada nodo tenga la oportunidad de cambiar su valor aproximadamente una vez por paso.
    //epsilon es donde almacenaremos las configuraciones que la red debe recordar y mu correrá estas configuraciones guardadas.
    //a es el valor promedio de las neuronas de cada patrón mu.
    //Delta_H, H_anterior y H_posterior sirven para calcular el cambio de energía de un paso Monte Carlo a otro.
    //solapamiento almacena el factor de solapamiento del sistema respecto a cada configuración guardada
    short int s[N][N], epsilon[N][N][P], n, m, k, l, mu, n_cambio, m_cambio, x;
    long int i, j, P_recuperados[P];
    double p, prob, Delta_H, H_anterior, H_posterior, a[P], theta[N][N], solapamiento[P];

    //Vamos a darle los valores iniciales al estado inicial del sistema

    for (mu = 0; mu < P; mu++)
    {
        P_recuperados[mu] = 0;
    }

    for ( x = 0; x < P; x++)
    {
    
    if (inicial == 1) //Caso todos los valores son 1. Se sobreescribe el fichero inicial.
    {
        ofstream fichero_inicial(ruta_fichero_sistema_inicial);

        for (n = 0; n < N; n++)
        {
            for (m = 0; m < N; m++)
            {
                s[n][m] = 1;
                fichero_inicial << s[n][m] << "\t";
                fichero_estados << s[n][m] << "\t";
            }
            fichero_inicial << endl;
            fichero_estados << endl;
        }

        fichero_inicial.close();   
    }

    else if (inicial == 0) //Caso todos los valores son 0. Se sobreescribe el fichero inicial.
    {
        ofstream fichero_inicial(ruta_fichero_sistema_inicial);

        for (n = 0; n < N; n++)
        {
            for (m = 0; m < N; m++)
            {
                s[n][m] = 0;
                fichero_inicial << s[n][m] << "\t";
                fichero_estados << s[n][m] << "\t";
            }
            fichero_inicial << endl;
            fichero_estados << endl;
        }

        fichero_inicial.close();
    }

    else if (inicial == -1) //Caso en el que los valores iniciales son aleatorios. Se sobreescribe el fichero inicial.
    {
        ofstream fichero_inicial(ruta_fichero_sistema_inicial);

        for (n = 0; n < N; n++)
        {
            for (m = 0; m < N; m++)
            {
                prob = r_distribution(generator);
                if (prob>=0.5)
                {
                    s[n][m] = 1;
                }
                else if (prob<0.5)
                {
                    s[n][m] = 0;
                }
                fichero_inicial << s[n][m] << "\t";
                fichero_estados << s[n][m] << "\t";
            }
            fichero_inicial << endl;
            fichero_estados << endl;
        }

        fichero_inicial.close();
    }
    
    else //Caso en el que los valores iniciales son los del fichero.
    {
        ifstream fichero_inicial(ruta_fichero_sistema_inicial);

        if (!fichero_inicial)
        {
            cout << "No se pudo abrir el fichero de valores iniciales del sistema.";
            return 0;
        }
        
        for (n = 0; n < N; n++)
        {
            for (m = 0; m < N; m++)
            {
                fichero_inicial >> s[n][m];
                fichero_estados << s[n][m] << "\t";
            }

            fichero_estados << endl;
        }

        fichero_inicial.close();
    }


    //Este ciclo for se necesita para posteriormente poder calcular el hamiltoniano

    for (mu = 0; mu < P; mu++)
    {
        //Primero metemos en la variable mu las configuraciones guardadas del archivo config_guardadas

        for ( n = 0; n < N; n++)
        {
            for ( m = 0; m < N; m++)
            {
                config_guardadas >> epsilon[n][m][mu];
                if (reps != 1 )
                {
                    if (x==mu)
                    {
                        s[n][m] = epsilon[n][m][mu];
                    }
                    
                }
                
            }
            
        }

        //Una vez determinadas las configuraciones guardadas, calculamos la a[mu] de cada una. Es el valor promedio de los nodos para cada configuración.

        a[mu] = 0.0;

        for ( n = 0; n < N; n++)
        {
            for ( m = 0; m < N; m++)
            {
                a[mu] = a[mu] + epsilon[n][m][mu];
            }
            
        }

        a[mu] = a[mu]/(1.0*N*N);
        
        //Aprovechamos el ciclo en mu para calcular el solapamiento del sistema con cada configuración

        calculo_solapamiento(solapamiento,s,epsilon,a,mu);

        fichero_solapamientos << " " << solapamiento[mu];

    }

    fichero_solapamientos  << endl;

    //Dejamos calculados los umbrales de disparo de cada nodo, theta[n][m]
    for (n = 0; n < N; n++)
    {
        for (m = 0; m < N; m++)
        {
            theta[n][m] = 0;
            
            for (k = 0; k < N; k++)
            {
                for (l = 0; l < N; l++)
                {
                    theta[n][m] = theta[n][m] + omega(n, m, k, l, epsilon, a);
                }
                
            }

            theta[n][m] = 0.5*theta[n][m];
            
        }
        
    }
    
    //Ya tenemos los epsilon de cada nodo en cada patrón, tenemos las a de cada patrón
    //tenemos una función para obtener la omega entre cada par de nodos teniendo en cuenta todos los patrones
    //y por último hemos obtenido theta para cada nodo
    //Calculemos ahora el hamiltoniano en el momento inicial de la red neuronal 

    H_anterior = 0.0;

    for ( n = 0; n < N; n++)
    {
        for ( m = 0; m < N; m++)
        {
            
            H_anterior = H_anterior + theta[n][m]*s[n][m];

            for ( k = 0; k < N; k++)
            {
                for ( l = 0; l < N; l++)
                {
                    H_anterior = H_anterior - 0.5*(omega(n,m,k,l,epsilon,a)*s[n][m]*s[k][l]);
                }
            }
        }
    }


    /***********************************************************************************************************************/
    //Ya hemos inicializado la matriz de nuestros estados.
    //Vamos a definir lo que hace cada paso Monte Carlo y calculamos los valores de cada partícula del sistema s.

    cout << endl << "Ejecutando programa ";
    if (reps != 1)
    {
        cout << "número " << x;
    }
    cout << endl;

    for (i = 0; i < Monte_Carlo; i++)
    {   

        fichero_solapamientos << i+1;
        
        for (j = 0; j < N*N; j++)
        {
            //Elegimos un nodo al azar
            n_cambio = i_distribution(generator); //Fila
            m_cambio = i_distribution(generator); //Columna

            //Necesitamos calcular el nuevo hamiltoniano con el cambio efectuado.
            //Para ello eliminamos el efecto de la partícula seleccionada, en el su antiguo estado en el Hamiltoniano, y añadimos el efeco del nuevo estado de la partícula
            H_posterior = H_anterior - theta[n_cambio][m_cambio]*s[n_cambio][m_cambio];

            for ( k = 0; k < N; k++)
            {
                for ( l = 0; l < N; l++)
                {
                    H_posterior = H_posterior + 0.5*omega(n_cambio,m_cambio,k,l,epsilon,a)*s[n_cambio][m_cambio]*s[k][l];
                }

            }

            s[n_cambio][m_cambio] = 1-s[n_cambio][m_cambio];

            for ( k = 0; k < N; k++)
            {
                for ( l = 0; l < N; l++)
                {
                    H_posterior = H_posterior - 0.5*omega(n_cambio,m_cambio,k,l,epsilon,a)*s[n_cambio][m_cambio]*s[k][l];
                }

            }
            
            H_posterior = H_posterior + theta[n_cambio][m_cambio]*s[n_cambio][m_cambio];

            Delta_H = H_posterior - H_anterior;

            p = exp(-Delta_H/(1.0*T));
            if (p>1)
            {
                p = 1;
            }

            //Calculamos una probabilidad de que ocurra el cambio, y si nuestra p es mayor, ocurre el cambio de signo en s[n][m]
            prob = r_distribution(generator);
            if (prob > p)
            {
                s[n_cambio][m_cambio] = 1-s[n_cambio][m_cambio];
            }
            else
            {
                H_anterior = H_posterior;
            }

        }

        //Calculamos el solapamiento de la red con cada configuración

        for ( mu = 0; mu < P; mu++)
        {
            calculo_solapamiento(solapamiento,s,epsilon,a,mu);

            fichero_solapamientos << " " << solapamiento[mu];
            
        }
    
        fichero_solapamientos  << endl;

        //Metemos el estado resultante en el fichero al acabar el paso Monte_Carlo

        for (n = 0; n < N; n++)
        {
            for (m = 0; m < N; m++)
            {
                fichero_estados << s[n][m] << "\t";
            }
            fichero_estados << endl; 
        }

        //Mostramos al usuario el progreso del programa
        if ((i%((Monte_Carlo+1)/5)) == 0)
        {
            if (reps == 1)
            {
                cout << "Paso Monte_Carlo numero " << i << "/" << Monte_Carlo << endl;
            }
        }
    }
    
    for (mu = 0; mu < P; mu++)
    {

        if (solapamiento[mu] >= 0.75)
        {
            P_recuperados[mu] = P_recuperados[mu] + 1;
        }
        else if (solapamiento[mu] <= -0.75)
        {
            P_recuperados[mu] = P_recuperados[mu] + 1;
        }
        
    }

    if (reps == 1 )
    {
        cout << "Paso Monte_Carlo numero " << i << "/" << Monte_Carlo << endl; 
    }
    
    fichero_estados.close();
    fichero_solapamientos.close();

    if (reps == 1)
    {
        x = P;
    }

    }

    if (reps != 1)
    {
        for (mu = 0; mu < P; mu++)
        {
            cout << "Patrón " << mu << ": ";
            if (P_recuperados[mu] >= 1)
            {
                cout << "Sí se recupera." << endl;
            }
            else
            {
                cout << "No se recupera!!!" << endl;
            }
        }
    
    }

    return 0;
}

//Calculamos omega en cada momento porque si quisiéramos guardar sus valores necesitaríamos un array de N^4 elementos (demasiado)
double omega(short int n, short int m, short int k, short int l, short int epsilon[][N][P], double a[])
{
    if ((n == k)&&(m == l))
    {
        return 0.0;
    }

    else
    {
        double valor = 0.0;
        short int mu_prima;

        for (mu_prima = 0; mu_prima < P; mu_prima++)
        {
            valor = valor + (epsilon[n][m][mu_prima] - a[mu_prima])*(epsilon[k][l][mu_prima] - a[mu_prima]);
        }

        return (1.0*valor)/(1.0*N*N);
    }
    
}

void calculo_solapamiento(double solapamiento[], short int s[][N], short int epsilon[][N][P], double a[], short int mu)
{
    short int n_prima, m_prima;

    solapamiento[mu] = 0.0;

    for ( n_prima = 0; n_prima < N; n_prima++)
    {
        for ( m_prima = 0; m_prima < N; m_prima++)
        {
            solapamiento[mu] = solapamiento[mu] + (epsilon[n_prima][m_prima][mu] - a[mu])*(s[n_prima][m_prima] - a[mu]);
        }
            
    }

    solapamiento[mu] = (1.0*solapamiento[mu])/(1.0*N*N*a[mu]*(1.0 - a[mu]));

    return;
}