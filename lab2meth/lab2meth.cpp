#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class Optimization
{
protected:
    float const a = 0.25f; // Нижняя граница отрезка неопределённости
    float const b = 2.0f; // Верхняя граница отрезка неопределённости

    /*

        Метод расчёта целевой функции, заданной в условии.

        Вариант 29.

    */
    float f(float x)
    {
        return x * x + 1.0f / x;
    }

    /*

        Метод расчёта производной целевой функции.

    */
    float f_(float x)
    {
        return 2 * x - 1 / (x * x);
    }

    /*

        Метод расчёта двойной производной целевой функции.

    */
    float f__(float x)
    {
        return 2 / pow(x, 3) + 2;
    }

public:
    /*

        Метод основного алгоритма.

        Неопределён для последующего полиморфизма.

    */
    virtual void algorithm(int N) = 0;
};

class FibonacciNumberMethod : Optimization
{
protected:
    vector <int> fibonacci = { 0, 1 }; // Вектор числел Фибоначчи. Минимальное кол-во чисел - 2.

    /*

        Метод расчёта чисел Фибоначчи.

        Принимаемый на вход параметр N - кол-во чисел.
        N также включает в себя первые два числа {0, 1}.

    */
    void fibonacciCalculation(int N)
    {
        for (int i = 0; i < N - 2; i++)
        {
            fibonacci.push_back(fibonacci[i] + fibonacci[i + 1]);
        }
    }
public:
    void algorithm(int N) override
    {
        fibonacciCalculation(N);

        float x1, x2, _x, y1, y2, _y, E_calculated, E_guaranteed, _E1;
        float E = (b - a) / (2.0f * fibonacci[N - 1]);
        float delta = E / 100.0f;
        float a_changeable = a, b_changeable = b;

        int k = 2;

        x1 = a_changeable + (b_changeable - a_changeable) * fibonacci[N - 1 - 2] / fibonacci[N - 1];
        y1 = f(x1);

        x2 = a_changeable + (b_changeable - a_changeable) * fibonacci[N - 1 - 1] / fibonacci[N - 1];
        y2 = f(x2);

        E_calculated = (b_changeable - a_changeable) / (2 * fibonacci[N - 1]) + delta / 2;

        do
        {
            if (y1 < y2)
            {
                b_changeable = x2;
                x2 = x1;
                y2 = y1;
                x1 = a_changeable + b_changeable - x2;
                y1 = f(x1);
            }
            else
            {
                a_changeable = x1;
                x1 = x2;
                y1 = y2;
                y1 = y2;
                x2 = a_changeable + b_changeable - x1;
                y2 = f(x2);
            }

            k++;

        } while (k < N - 1);

        if (y1 < y2)
        {
            b_changeable = x2;
            x2 = x1;
            y2 = y1;
        }
        else
            a_changeable = x1;

        x1 = x2 - delta;
        y1 = f(x1);

        if (y1 < y2)
            b_changeable = x2;
        else
            a_changeable = x1;

        _x = (a_changeable + b_changeable) / 2;
        _y = f(_x);

        E_guaranteed = (b_changeable - a_changeable) / 2;
        _E1 = f_(_x);

        cout << "~x: " << _x << "   ~y: " << _y << "    Е-гарантированное: " << E_guaranteed << "   ~Е1: " << _E1 << "  Е0: " << E_calculated << "\n";
    }
};

class MidpointMethod : Optimization
{
public:
    void algorithm(int N) override
    {
        float x, _x, y, _y, E_guaranteed, _E1, z;
        float E_calculated = (1 / 2) * ((b - a) / 2 * N);
        float a_changeable = a, b_changeable = b;
        int k = 0;
        bool break_flag = false;

        do
        {
            x = (a_changeable + b_changeable) / 2;
            z = f_(x);
            k++;

            if (z == 0.0f)
            {
                E_guaranteed = 0.0f;
                _x = x;
                _y = f(_x);
                _E1 = 0;
                break_flag = true;
                break;
            }

            if (z > 0)
            {
                b_changeable = x;
            }
            else
            {
                a_changeable = x;
            }
        } while (k < N);

        if (!break_flag)
        {
            E_guaranteed = (b_changeable - a_changeable) / 2;
            _x = (a_changeable + b_changeable) / 2;
            _y = f(_x);
            _E1 = abs(f_(_x));
        }

        cout << "~x: " << _x << "   ~y: " << _y << "    k: " << k << "   ~Е1: " << _E1 << "   E-гарантированное: " << E_guaranteed << "  Е0: " << E_calculated << "\n";
    }
};

class ChordMethod : Optimization
{
public:
    void algorithm(int N) override
    {
        float x, _x, y, _y, E_guaranteed, E_calculated, _E1, z;
        float a_changeable = a, b_changeable = b;
        float z1 = f_(a_changeable), z2 = f_(b_changeable);
        int k = 2;
        bool break_flag = false;

        do
        {
            x = a_changeable - (z1 / (z2 - z1)) * (b_changeable - a_changeable);
            z = f_(x);
            k++;

            if (z == 0)
            {
                _x = x;
                _y = f(_x);
                E_guaranteed = 0;
                _E1 = 0;
                break_flag = true;
                break;
            }
            
            if (z > 0)
            {
                b_changeable = x;
                z2 = z;
            }
            else
            {
                a_changeable = x;
                z1 = z;
            }

        } while (k < N);

        if (!break_flag) 
        {
            _x = x;
            _y = f(_x);
            E_guaranteed = b_changeable - a_changeable;
            _E1 = abs(z);
        }

        cout << "~x: " << _x << "   ~y: " << _y << "    k: " << k << "   ~Е1: " << _E1 << "   E-гарантированное: " << E_guaranteed << "\n";
    }
};

class TangentMethod : Optimization
{
public:
    void algorithm(int N) override
    {
        float x, _x, y, _y, E_guaranteed, _E1, z;
        float a_changeable = a, b_changeable = b;
        float y1 = f(a_changeable), y2 = f(b_changeable);
        float z1 = f_(a_changeable), z2 = f_(b_changeable);
        int k = 4;
        bool break_flag = false;

        do
        {
            x = ((z2 * b_changeable - z1 * a_changeable) - (y2 - y1)) / (z2 - z1);
            y = f(x);
            z = f_(x);
            k += 2;

            if (z == 0)
            {
                _x = x;
                _y = y;
                E_guaranteed = 0;
                _E1 = 0;
                break_flag = true;
                break;
            }

            if (z > 0)
            {
                b_changeable = x;
                y2 = y;
                z2 = z;
            }
            else
            {
                a_changeable = x;
                y1 = y;
                z1 = z;
            }
        } while (k < N);

        if (!break_flag)
        {
            _x = x;
            _y = y;
            E_guaranteed = b_changeable - a_changeable;
            _E1 = abs(z);
        }

        cout << "~x: " << _x << "   ~y: " << _y << "    k: " << k << "   ~Е1: " << _E1 << "   E-гарантированное: " << E_guaranteed << "\n";
    }
};

class NewtonMethod : Optimization
{
public:
    void algorithm(int N) override
    {
        float _x, y, _y, E_guaranteed, _E1;
        float a_changeable = a, b_changeable = b;
        float x = a_changeable, z = f_(x), u = f__(x);
        int k = 2;

        while ((k < N) && (z != 0))
        {
            x = x - z / u;
            z = f_(x);
            u = f__(x);
            k += 2;
        }

        _x = x;
        _y = f(_x);
        _E1 = abs(z);

        cout << "~x: " << _x << "   ~y: " << _y << "    k: " << k << "   ~Е1: " << _E1 << "\n";
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Метод чисел Фибоначчи:\n\n";

    FibonacciNumberMethod* fibonacciNumberMethod = new FibonacciNumberMethod();

    for (int i = 1; i <= 5; i++)
        fibonacciNumberMethod->algorithm(i * 10);

    fibonacciNumberMethod->~FibonacciNumberMethod();

    delete fibonacciNumberMethod;

    cout << "\nМетод средней точки:\n\n";

    MidpointMethod* midpointMethod = new MidpointMethod();

    for (int i = 1; i <= 5; i++)
        midpointMethod->algorithm(i * 10);

    midpointMethod->~MidpointMethod();

    delete midpointMethod;

    cout << "\nМетод хорд:\n\n";

    ChordMethod* chordMethod = new ChordMethod();

    for (int i = 1; i <= 5; i++)
        chordMethod->algorithm(i * 10);

    chordMethod->~ChordMethod();

    delete chordMethod;

    cout << "\nМетод касательных:\n\n";

    TangentMethod* tangentMethod = new TangentMethod();

    for (int i = 1; i <= 5; i++)
        tangentMethod->algorithm(i * 10);

    tangentMethod->~TangentMethod();

    delete tangentMethod;

    cout << "\nМетод Ньютона:\n\n";

    NewtonMethod* newtonMethod = new NewtonMethod();

    for (int i = 1; i <= 5; i++)
        newtonMethod->algorithm(i * 10);

    newtonMethod->~NewtonMethod();

    delete newtonMethod;
}