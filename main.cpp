#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


/* *** Exemplo para demonstrar o uso de Threads e Mutexes / Funções lambdas (funções anonimas) ***
 *
 * //////////////////////////////////////////////////////////////////////////////////////////////////
 * Para se usar um thread, basta inserir o header <thread>.
 * a thread é executada em uma função especifica, portanto ao se instanciar um objeto
 * thread deve-se passar uma função como argumento, seguido dos argumentos da própria função.
 * para se passar argumentos por referencia para uma thread não basta colocar o operador "&"
 * no protótipo da função, ao passar o argumento para a thread ele deve ser utilizado com o
 * std::ref(argumento), c.c a thread vai realizar uma cópia do parâmetro.
 *
 * Se uma thread  T1 usa um recurso R comum a outra thread T2, isso pode gerar conflitos, na medida
 * em que T1 e T2 podem tentar acessar R exatamente ao mesmo tempo, para que este ero seja corrigido
 * é necessário um mecanismo que "bloqueie" o recurso em comum, de forma que se T1 o esta usando,
 * T2 não pode usar o mesmo recurso, isso é feito pelo mutex (header <mutex>)
 * basta declarar um mutex, e usar a propriedade .lock() e .unlock() antes e depois das partes em que
 * os recursos em comum serão utilizados.
 *
 * O código thread_example_1() instancia duas thread que usam dois recursos em comum,
 * um iterador e um vetor, as duas têm um laço de repetição que incrementa o iterador,
 * e o número de incrementos realizados em cada uma é armazenado nas posições 0 e 1 do
 * vetor, como as duas compartilham o mesmo recurso, como o iterador vai até 1000, a soma
 * das quantidades executadas na thread1 e na 2 devem ser 1000, pois cada um altera o valor do contador
 * independentemente da outra.
 *
 * Dentro da função evaluate rects existe um mutex para evitar o acesso simultaneo das duas thread,
 * se os mutexes forem comentads, no console podem aparecer casos do tipo:
 * .
 * .
 * 0
 * 0
 * 10 <- neste caso as duas thread acessaram o recurso ao mesmo tempo
 * 1
 * 0
 * .
 *
 * Quando os mutexes são descomentados, isso não ocorre;
 * ///////////////////////////////////////////////////////////////
 *
 *
 * Uma função labda é uma função que não precisa de nomes e é associada
 * a uma variável. A sintaxe é composta de:
 * []()->retorno{};
 * [var1, var2] = Captura de variáveis
 * (arg1, arg2) = Argumentos
 * {};          = Código
 *
 * A função lambda dve ser armazeada em uma varíavel, de forma que usamos ou
 * o "auto" (que aceita qualquer coisa), ou as varíaveis do tipo função
 * std::function<retorno(arg1, arg2)>
 *
 *
 * FUNÇÕES LABDA https://www.youtube.com/watch?v=j6VqKrBc5Ew
 * VECTOR THREADS https://thispointer.com/c11-how-to-create-vector-of-thread-objects/
 *
 */

using namespace std;

static mutex mtx;

void evaluate_rects(vector<int> &shared_rects, int rect_index, int &shared_iteration)
{
    int a = 0;
    while(shared_iteration < 1000)
    {
        mtx.lock();
        shared_rects[rect_index] += 1;
        shared_iteration++;
        cout << rect_index << "\n";
        mtx.unlock();
    }
}

void thread_example_1()
{
    int stop_criteria = 0;
    vector<int> rects{0,0};
    int rect_0 = 0;
    int rect_1 = 1;

    thread t1(evaluate_rects,std::ref(rects),rect_0,std::ref(stop_criteria));
    thread t2(evaluate_rects,std::ref(rects),rect_1,std::ref(stop_criteria));

    t1.join();
    t2.join();

    cout << rects[0] << "-" << rects[1];
}

void vector_thread_example_2()
{
    /*
     * Para se criar um vetor de threads, as funções que são passadas para as thread devem
     * ser do tipo lambda
     */

    function<void(vector<int> &v_rects,int rect_number, int &stop_criteria)> f_lambda_evaluate_rects = [](vector<int> &v_rects,int rect_number,int &stop_criteria)
    {
        while(stop_criteria < 1000)
        {
            mtx.lock();
            v_rects[rect_number] += 1;
            stop_criteria++;
            cout << stop_criteria << "\n";
            mtx.unlock();
        }
    };

    int n_threads = 4;
    int stop_criteria = 0;
    vector<int> v_rects(n_threads,0);
    vector<std::thread> v_threads;



    for(int i = 0; i < n_threads; i++)
    {
        v_threads.push_back(thread(f_lambda_evaluate_rects,ref(v_rects),i,ref(stop_criteria)));
    }

    for(int i = 0; i < v_threads.size() ; i++)
    {
        if(v_threads[i].joinable())
        v_threads[i].join();
    }

    for(int i=0;i < n_threads; i++)
    {
        if(i == 0)
        {
            cout << "[" << v_rects[i];
        }else if(i == n_threads-1)
        {
            cout << ";" <<v_rects[i] << "]";
        }else
        {
            cout << ";" <<v_rects[i];
        }


    }

}

void lambda_example_1()
{
     //Lambda que recebe dois numeros e retorna o maior deles
    function<int(int,int)> f = [](int n1, int n2) -> int
    {
        if(n1 > n2) return n1;
        else return n2;
    };
    cout << "O maior número entre 2 e 6 é :" << f(2,6);
}

void exemplo_variael_funcao()
{
    /*
     * Chama uma função por meio de uma varíavel
     */

    function<void()> f;    //variavel do tipo função
    f = thread_example_1;  //varíavel recebe uma função
    f();
}

int main()
{

    vector_thread_example_2();
    //lambda_example_1();
    //thread_example_1();
    //thread_example_2();

  return 0;
}
