#include <iostream>
using namespace std;

class SparseMatrix{

    private: //Esta en private para que se pueda acceder solo desee esta clase

    struct celdas{
        int val,col,row;
        celdas* next,*prev,*up,*down;
        celdas(int va,int co,int ro): val(va), col(co),row(ro),next(nullptr),prev(nullptr),up(nullptr),down(nullptr) {}
    };

    struct matriz {
        int index;
        matriz* sig;
        celdas* cel;
        matriz(int in): index(in), sig(nullptr), cel(nullptr) {}
    };

    struct FirstSentinel{
        matriz* row;
        matriz* col;
        FirstSentinel(): row(nullptr), col(nullptr) {}
    };

    FirstSentinel *head=new FirstSentinel(); 

//Creamos la funcion de Crear Fila y Columna que tambien buscara si ya hay una en esa posicion
    matriz* CrearEncontrarNodoFila(int fila){//Retornara el puntero del nodo que creo o busca
        if (head->row==nullptr){ //No hay fila
            head->row=new matriz(fila);
            return head->row;
        }
        if(head->row->index == fila)return head->row;//Solo hay un nodo matriz y justo es el que buscamos
        if(fila<head->row->index){//si el numero de fila es menor al primer elemento de matriz, entonces es el nodo que le sigue
            matriz* nuevo =new matriz(fila);
            nuevo->sig=head->row;
            head->row=nuevo;
            return nuevo;
        }
        matriz* temp=head->row, *prevtemp=nullptr;
        while(temp!=nullptr && temp->index < fila){
            prevtemp = temp;
            temp=temp->sig;//Avanzara un paso, pero siempre prevtemp quedara uno antes para enlazar si hace falta
        }
        if (temp!=nullptr && temp->index==fila)return temp;//Si ya existe,no hacer nada
        
        matriz* NuevaMatriz=new matriz(fila);
        NuevaMatriz->sig=temp;
        prevtemp->sig=NuevaMatriz;
        return NuevaMatriz;
        }
    
    matriz* CrearEncontrarNodoColumna(int columna){//Solo copie y pegue la funcion de nodo fila, solo cambio los nombres de row por col y fila por columna
        if (head->col==nullptr){ //No hay fila
            head->col=new matriz(columna);
            return head->col;
        }
        if(head->col->index == columna)return head->col;//Solo hay un nodo matriz y justo es el que buscamos
        if(columna<head->col->index){
            matriz* nuevo =new matriz(columna);
            nuevo->sig=head->col;
            head->col=nuevo;
            return nuevo;
        }
        matriz* temp=head->col, *prevtemp=nullptr;
        while(temp!=nullptr && temp->index < columna){
            prevtemp = temp;
            temp=temp->sig;//Avanzara un paso, pero siempre prevtemp quedara uno antes para enlazar si hace falta
        }
        if (temp!=nullptr && temp->index==columna)return temp;//Si ya existe,no hacer nada
        
        matriz* NuevaMatriz=new matriz(columna);
        NuevaMatriz->sig=temp;
        prevtemp->sig=NuevaMatriz;
        return NuevaMatriz;
        
    }

    celdas* CrearBuscarCeldas(int dato, int c, int f){
        matriz*fi=CrearEncontrarNodoColumna(c);//Lo llamamos fil porque se desplazara de una fila a otra a pesar de estar en la columna.
        matriz*co=CrearEncontrarNodoFila(f);
        if(fi->cel==nullptr && co->cel==nullptr){//No existe el nodo de la celda y tanto esa fila como esa columna estan vacias(de celdas)
            celdas* NuevaCel=new celdas(dato,c,f);
            fi->cel=NuevaCel;
            co->cel=NuevaCel;
            return NuevaCel;
        }
        if(fi->cel==nullptr){// La columna vacia, pero ya que hay return, es imposible que la fila este vacia y hay al menos una celda en la fila
            celdas* NuevaCel=new celdas(dato,c,f);//Si la columna esta vacia, es obvio que aun no existe esa celda
            fi->cel=NuevaCel;//Pienso colocar un bucle while para encontrar la posicion en la que introducir 
            celdas* temp= co->cel;
            while(temp->col < c&& temp->next!=nullptr){
                temp=temp->next;
            }
            if(temp->col<c){//Caso 1: Las celdas terminan antes del nuevo nodo
                temp->next=NuevaCel;
                NuevaCel->prev=temp;
            }else if(temp->prev!=nullptr){
                temp->prev->next=NuevaCel;
                NuevaCel->prev=temp->prev;
                temp->prev=NuevaCel;
                NuevaCel->next=temp;
            }else{
                co->cel=NuevaCel;
                temp->prev=NuevaCel;
                NuevaCel->next=temp;
            }
            return NuevaCel;
        }
        if(co->cel==nullptr){// La fila vacia, pero hay al menos una celda en la columna
            celdas* NuevaCel=new celdas(dato,c,f);//Si la fila esta vacia, es obvio que aun no existe esa celda
            co->cel=NuevaCel;
            celdas* temp= fi->cel;
            while(temp->row < f&& temp->down!=nullptr){
                temp=temp->down;
            }//Si temp->down ya no es mayor que f, es porque ya esta despues del lugar donde enlazara el nodo nuevo
            if(temp->row<f){//Caso 1: Las celdas terminan antes del nuevo nodo
                temp->down=NuevaCel;
                NuevaCel->up=temp;
            }else if(temp->up!=nullptr){
                temp->up->down=NuevaCel;
                NuevaCel->up=temp->up;
                temp->up=NuevaCel;
                NuevaCel->down=temp;
            }else{
                fi->cel=NuevaCel;
                temp->up=NuevaCel;
                NuevaCel->down=temp;
            }
            return NuevaCel;
        }
        //Si ninguno de esos casos se cumplio es porque la fila y columna en la que insertaremos las celdas tienen al menos un elemento
        //Debemos ver si es que la celda ya existe.
        celdas* tempco= co->cel;//Se llama tempco porque ira de columna en columna
        celdas* tempfi= fi->cel;
        while(tempco->col < c&& tempco->next!=nullptr){
            tempco=tempco->next;
        }
        if (tempco->col==c)return tempco;//Si ya existe
        while(tempfi->row < f&& tempfi->down!=nullptr){
            tempfi=tempfi->down;
        }
        if (tempfi->row==f)return tempfi;//Si ya existe
        //PRIMERO CONECTAMOS EL NUEVO NODO CON LOS NODOS HORIZONTALES
        celdas* NuevaCel=new celdas(dato,c,f);
        if(tempco->col<c){//Caso 1: Las celdas terminan antes del nuevo nodo
            tempco->next=NuevaCel;
            NuevaCel->prev=tempco;
        }else if(tempco->prev!=nullptr){
            tempco->prev->next=NuevaCel;
            NuevaCel->prev=tempco->prev;
            tempco->prev=NuevaCel;
            NuevaCel->next=tempco;
        }else{
            co->cel=NuevaCel;
            tempco->prev=NuevaCel;
            NuevaCel->next=tempco;
        }
        //CONECTAMOS EL NUEVO NODO CON LOS NODOS VERTICALES
        if(tempfi->row<f){//Caso 1: Las celdas terminan antes del nuevo nodo
            tempfi->down=NuevaCel;
            NuevaCel->up=tempfi;
        }else if(tempfi->up!=nullptr){
            tempfi->up->down=NuevaCel;
            NuevaCel->up=tempfi->up;
            tempfi->up=NuevaCel;
            NuevaCel->down=tempfi;
        }else{
            fi->cel=NuevaCel;
            tempfi->up=NuevaCel;
            NuevaCel->down=tempfi;
        }
    return NuevaCel;        

    }
    public:

    void MoverCeldaPosicion(int nValor, int nCol, int nFil, int valor, int fil, int col){

    }
    //Funcion de Cambio de Posicion
    //Funcion de insertar fila
    //Funcion de insertar Columna
    //Funcion Suma,resta,etc

};