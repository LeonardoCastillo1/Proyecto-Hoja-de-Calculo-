#include <iostream>
using namespace std;

class SparseMatrix{

    private: //Esta en private para que se pueda acceder solo desee esta clase

    struct Celdas{
        int val,col,row;
        Celdas* next,*prev,*up,*down;
        Celdas(int va,int co,int ro): val(va), col(co),row(ro),next(nullptr),prev(nullptr),up(nullptr),down(nullptr) {}
    };

    struct Matriz {
        int index;
        Matriz* sig;
        Celdas* cel;
        Matriz(int in): index(in), sig(nullptr), cel(nullptr) {}
    };

    struct FirstSentinel{
        Matriz* row;
        Matriz* col;
        FirstSentinel(): row(nullptr), col(nullptr) {}
    };

    FirstSentinel *head=new FirstSentinel(); 

    struct HashNode{
        int key;
        Matriz* cabecera;
        HashNode* next;
        HashNode(int k, Matriz* c, HashNode* n):key(k),cabecera(c),next(n){}
    };
    static const int capacity = 10;
    HashNode * HashFilas[capacity];//Crearemos dos Arrays distintos, uno para las filas y otro para las columnas.  
    HashNode * HashColumnas[capacity];

    public:

    SparseMatrix(){
        for (int i=0;i<capacity;i++){
            HashFilas[i]=nullptr;
            HashColumnas[i]=nullptr;
        }
    }

    private:
//HASH TABLE: IMPLEMENTACION
        //USAREMOS EL METODO DE CHAINING, PARA ACCEDER A LOS NODOS DE LAS CABECERAS EN EL MENOR TIEMPO POSIBLE
    int HashFunction (int key){
        return key%capacity;
    }

    Matriz* BuscarFilaHash(int fila){//La idea es usar la hash table para simplificar, por lo que la key sera la fila
        int index= HashFunction(fila);
        HashNode* temp= HashFilas[index];
        while(temp!=nullptr){
            if(temp->key ==fila)return temp->cabecera;//Si el valor es la fila, es porque la fila existe, por lo que retornamos el puntero de ese nodoHash hacia el nodo Matriz
            temp=temp->next;
        }
        return nullptr;
    }
    Matriz* BuscarColumnaHash(int columna){
        int index=HashFunction(columna);
        HashNode* temp=HashColumnas[index];
        while(temp!=nullptr){
            if(temp->key==columna)return temp->cabecera;
            temp=temp->next;
        }
        return nullptr;
    }

    void InsertarFilaHash(int fila,Matriz* NodFil){
        int index=HashFunction(fila);
        HashNode* temp=HashFilas[index];
        while(temp!=nullptr){
            if (temp->key==fila){
                temp->cabecera=NodFil;//Actualiza si ya existe
                return;
            }
            temp=temp->next;
        }
        HashNode* NuevoNodoHash=new HashNode(fila,NodFil,nullptr);
        NuevoNodoHash->next=HashFilas[index];
        HashFilas[index]=NuevoNodoHash;
    }
    void InsertarColumnaHash(int columna,Matriz* NodCol){
        int index=HashFunction(columna);
        HashNode* temp=HashColumnas[index];
        while(temp!=nullptr){
            if (temp->key==columna){
                temp->cabecera=NodCol;//Actualiza si ya existe
                return;
            }
            temp=temp->next;
        }
        HashNode* NuevoNodoHash=new HashNode(columna,NodCol,nullptr);
        NuevoNodoHash->next=HashColumnas[index];
        HashColumnas[index]=NuevoNodoHash;
    }
    void EliminarNodoFilaHash(int fila){
        int index=HashFunction(fila);
        HashNode*temp=HashFilas[index];
        HashNode*prevtemp=nullptr;
        while(temp!=nullptr && temp->key !=fila){//se rompe cuando es nulo o es el elemento que buscamos
            prevtemp=temp;
            temp=temp->next;
        }
        if (temp==nullptr)return;
        if(prevtemp==nullptr){//Caso en el que temp desde un inicio estuvo en el nodo deseado, por eso prevtemp se mantuvo nullptr
               HashFilas[index]=temp->next;
        } 
        else{//Caso en el que temp se desplazo y no es nulo, osea es el nodo donde esta la fila que buscamos
            prevtemp->next=temp->next;
        }
        delete temp;
    }
    void EliminarNodoColumnaHash(int columna){
        int index=HashFunction(columna);
        HashNode*temp=HashColumnas[index];
        HashNode*prevtemp=nullptr;
        while(temp!=nullptr && temp->key !=columna){//se rompe cuando es nulo o es el elemento que buscamos
            prevtemp=temp;
            temp=temp->next;
        }
        if (temp==nullptr)return;
        if(prevtemp==nullptr){//Caso en el que temp desde un inicio estuvo en el nodo deseado, por eso prevtemp se mantuvo nullptr
               HashColumnas[index]=temp->next;
        } 
        else{//Caso en el que temp se desplazo y no es nulo, osea es el nodo donde esta la fila que buscamos
            prevtemp->next=temp->next;
        }
        delete temp;
    }



//HASH TABLE: FIN DE IMPLEMENTACION
//Implementacion Matrices Dispersas

//Creamos la funcion de Crear Fila y Columna que tambien buscara si ya hay una en esa posicion
    Matriz* CrearEncontrarNodoFila(int fila){//Retornara el puntero del nodo que creo o busca
        Matriz* EncontrarConHash=BuscarFilaHash(fila);//Buscamos en nuestra tabla hash si tenemos el elemento, si no devolvera un nullptr
        if(EncontrarConHash!=nullptr)return EncontrarConHash;

        if (head->row==nullptr){ //No hay fila
            head->row=new Matriz(fila);
            InsertarFilaHash(fila,head->row);
            return head->row;
        }
        if(head->row->index == fila){
            InsertarFilaHash(fila,head->row);
            return head->row;//Solo hay un nodo matriz y justo es el que buscamos
        }
        if(fila<head->row->index){//si el numero de fila es menor al primer elemento de matriz, entonces es el nodo que le sigue
            Matriz* nuevo =new Matriz(fila);
            nuevo->sig=head->row;
            head->row=nuevo;
            InsertarFilaHash(fila,nuevo);
            return nuevo;
        }
        Matriz* temp=head->row, *prevtemp=nullptr;
        while(temp!=nullptr && temp->index < fila){
            prevtemp = temp;
            temp=temp->sig;//Avanzara un paso, pero siempre prevtemp quedara uno antes para enlazar si hace falta
        }
        if (temp!=nullptr && temp->index==fila){
            InsertarFilaHash(fila,temp);//Por si acaso
            return temp;//Si ya existe,no hacer nada
    }
        Matriz* NuevaMatriz=new Matriz(fila);
        NuevaMatriz->sig=temp;
        prevtemp->sig=NuevaMatriz;
        InsertarFilaHash(fila,NuevaMatriz);
        return NuevaMatriz;
        }
    
    Matriz* CrearEncontrarNodoColumna(int columna){//Retornara el puntero del nodo que creo o busca
        Matriz* EncontrarConHash=BuscarColumnaHash(columna);//Buscamos en nuestra tabla hash si tenemos el elemento, si no devolvera un nullptr
        if(EncontrarConHash!=nullptr)return EncontrarConHash;
        if (head->col==nullptr){ //No hay fila
            head->col=new Matriz(columna);
            InsertarColumnaHash(columna,head->col);
            return head->col;
        }
        if(head->col->index == columna){
            InsertarColumnaHash(columna,head->col);//Por si acaso
            return head->col;//Solo hay un nodo matriz y justo es el que buscamos
        }
        if(columna<head->col->index){
            Matriz* nuevo =new Matriz(columna);
            nuevo->sig=head->col;
            head->col=nuevo;
            InsertarColumnaHash(columna,nuevo);
            return nuevo;
        }
        Matriz* temp=head->col, *prevtemp=nullptr;
        while(temp!=nullptr && temp->index < columna){
            prevtemp = temp;
            temp=temp->sig;//Avanzara un paso, pero siempre prevtemp quedara uno antes para enlazar si hace falta
        }
        if (temp!=nullptr && temp->index==columna){
            InsertarColumnaHash(columna,temp);
            return temp;//Si ya existe,no hacer nada
        }
        Matriz* NuevaMatriz=new Matriz(columna);
        NuevaMatriz->sig=temp;
        prevtemp->sig=NuevaMatriz;
        InsertarColumnaHash(columna,NuevaMatriz);
        return NuevaMatriz;
        
    }

    Celdas* CrearModificarBuscarCeldas(int dato, int c, int f){//Todo en una misma porque baja la complejidad. Puedes buscar en o(k), pero si no hay celda, llamar a otra funcion para que cree en esa posicion lo complica y haria otra vez la busqueda.
        Matriz*fi=CrearEncontrarNodoColumna(c);//Lo llamamos fil porque se desplazara de una fila a otra a pesar de estar en la columna.
        Matriz*co=CrearEncontrarNodoFila(f);
        if(fi->cel==nullptr && co->cel==nullptr){//No existe el nodo de la celda y tanto esa fila como esa columna estan vacias(de celdas)
            Celdas* NuevaCel=new Celdas(dato,c,f);
            fi->cel=NuevaCel;
            co->cel=NuevaCel;
            return NuevaCel;
        }
        if(fi->cel==nullptr){// La columna vacia, pero ya que hay return, es imposible que la fila este vacia y hay al menos una celda en la fila
            Celdas* NuevaCel=new Celdas(dato,c,f);//Si la columna esta vacia, es obvio que aun no existe esa celda
            fi->cel=NuevaCel;//Pienso colocar un bucle while para encontrar la posicion en la que introducir 
            Celdas* temp= co->cel;
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
            Celdas* NuevaCel=new Celdas(dato,c,f);//Si la fila esta vacia, es obvio que aun no existe esa celda
            co->cel=NuevaCel;
            Celdas* temp= fi->cel;
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
        Celdas* tempco= co->cel;//Se llama tempco porque ira de columna en columna
        Celdas* tempfi= fi->cel;
        while(tempco->col < c&& tempco->next!=nullptr){
            tempco=tempco->next;
        }
        if (tempco->col==c){
            tempco->val=dato;//Actualizamos el valor de la celda
            return tempco;//Si ya existe.Solo modificamos la celda/Actualiza valor
        }
        while(tempfi->row < f&& tempfi->down!=nullptr){
            tempfi=tempfi->down;
        }
        if (tempfi->row==f){
            tempfi->val=dato;//Actualizamos el valor de la celda
            return tempfi;//Si ya existe
        }
        //PRIMERO CONECTAMOS EL NUEVO NODO CON LOS NODOS HORIZONTALES
        Celdas* NuevaCel=new Celdas(dato,c,f);
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
    void EliminarNodoFilaVacio(Matriz* fila){
        if(fila==nullptr || fila->cel!=nullptr)return;


    }












    void EliminarCelda(Celdas* &Celda, Matriz* &Fila,Matriz* &Col){//Obtener celda una vez tenemos la ubicac
        if (Celda==nullptr || Fila== nullptr ||Col==nullptr)return;
        if (Celda==nullptr)return;
        //Primero Conexiones Verticales
        if (Celda->up!=nullptr)Celda->up->down=Celda->down;
        else{
            Col->cel=Celda->down;
        }
        if (Celda->down!=nullptr)Celda->down->up=Celda->up;
        //Ahora conexiones Horizontales
        if (Celda->next!=nullptr)Celda->next->prev=Celda->prev;
        if(Celda->prev!=nullptr)Celda->prev->next=Celda->next;
        else {
            Fila->cel=Celda->next;
        }
        delete Celda;
    }
    
    
    void EliminarRangoDCeldas(int ExtremoFilA,int ExtremoFilB,int ExtremoColA,int ExtremoColB){


    }


    public:
    //Suma Fila y columna sera con punteros. Pero SumaRango() recibe enteros, ya que puede pasar que si son punteros expremos, justo esas posiciones no existan, puede ccortar sin querer la suma
    int SumaFilaColumna(Celdas* CeldaA, Celdas *CeldaB){//A es extremo izquierdo/Superior y B es extremo derecho/Inferior
        if (CeldaA==nullptr || CeldaB==nullptr)return 0;
        int Sum=0;
        if (CeldaA->row == CeldaB->row){//Suma elementos de Fila
            Celdas*temp=CeldaA;
            while(temp!=nullptr && temp->col<=CeldaB->col){
                Sum+=temp->val;
                if (temp==CeldaB)return Sum;
                temp=temp->next;
            }
        }
        if (CeldaA->col == CeldaB->col){//Suma Elementos de Columna
            Celdas *temp=CeldaA;
            while(temp!=nullptr && temp->row <= CeldaB->row){
                Sum+=temp->val;
                if(temp==CeldaB)return Sum;
                temp=temp->down;
            }
        }
        return 0;//Estan en distinta fila/columna
    }
    int SumaRangoCeldas(int ExtremoFilA,int ExtremoFilB,int ExtremoColA,int ExtremoColB){//Los A son izq/Arrib, los B son dere/Abajo. Suma bloques de celdas
        if(head->col==nullptr || head->row==nullptr)return 0;
        int suma=0;
        Matriz* columna=head->col;
        while(columna!=nullptr && columna->index < ExtremoColA){
            columna=columna->sig;
        }
        while(columna!=nullptr && columna->index<=ExtremoColB){
            Celdas* temp=columna->cel;
            while(temp!=nullptr && temp->row<ExtremoFilA){
                temp=temp->down;
            }
            while(temp!=nullptr && temp->row<=ExtremoFilB){
                suma+=temp->val;
                temp=temp->down;
            }
            columna=columna->sig;
        }
        return suma;
    }


};