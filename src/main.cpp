#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>

using namespace std;

static const int WINDOW_SIZE_X = 800;
static const int WINDOW_SIZE_Y = 800;

static const double maxDistance = sqrt(WINDOW_SIZE_X * WINDOW_SIZE_X + WINDOW_SIZE_Y * WINDOW_SIZE_Y);

// путь к файлу профиля 1
static const string Profile1 = "D:\\cpp\\files\\Profile1.text";
// путь к файлу профиля 2
static const string Profile2 = "D:\\cpp\\files\\Profile2.text";

// буфер, хранящий координаты последней добавленной вершины
int lastAddPosBuf[2] = {0, 0};

// буфер кол-ва случайных точек
int lastRandoCntBuf[1] = {10};

// цвет фона
static sf::Color bgColor;
// значение цвета по умолчанию
float color[3] = {0.12f, 0.12f, 0.13f};

// задать цвет фона по вещественному массиву компонент
static void setColor(float *pDouble) {
    bgColor.r = static_cast<sf::Uint8>(pDouble[0] * 255.f);
    bgColor.g = static_cast<sf::Uint8>(pDouble[1] * 255.f);
    bgColor.b = static_cast<sf::Uint8>(pDouble[2] * 255.f);
}

// Точка
struct Point {
    // положение
    sf::Vector2i pos;

    // конструктор
    Point(const sf::Vector2i &pos) : pos(pos) {
    }

    // получить случайную точку
    static Point randomPoint() {
        return Point(sf::Vector2i(
                rand() % WINDOW_SIZE_X,
                rand() % WINDOW_SIZE_Y)
        );
    }
};

std::vector<Point> points;

struct triangle {
    Point p1;
    Point p2;
    Point p3;
    int i;
    triangle(const Point p1, const Point p2, const Point p3, int i) : p1(p1),p2(p2),p3(p3),i(i){
    }
};

triangle tr(sf::Vector2i(-10,-10),sf::Vector2i(-10,-10),sf::Vector2i(-10,-10),0);

struct line{
    Point p1;
    Point p2;
    line(const Point p1,const Point p2) : p1(p1),p2(p2){
    }
};

line l(sf::Vector2i(-10,-10),sf::Vector2i(-10,-10));

// загрузка из файла
void loadFromProfile(const int n) {
    string Profile;
    if(n==1)  Profile = Profile1;
    else  Profile = Profile2;

    std::cout << "preLoad " << Profile << std::endl;


    // открываем поток данных для чтения из файла
    std::ifstream input(Profile);

    std::cout << "preLoad2" << std::endl;

    // очищаем массив точек
    points.clear();

    if(!input.eof()) { input >> tr.p1.pos.x; input >> tr.p1.pos.y; }
    if(!input.eof()) { input >> tr.p2.pos.x; input >> tr.p2.pos.y; }
    if(!input.eof()) { input >> tr.p3.pos.x; input >> tr.p3.pos.y; }

    // пока не достигнут конец файла
    while (!input.eof()) {
        int x, y;
        input >> x; // читаем x координату
        input >> y; // читаем y координату

        std::cout << x << " " << y  << std::endl;

        // добавляем в динамический массив точку на основе прочитанных данных
        points.emplace_back(Point(sf::Vector2i(x, y)));
    }
    // закрываем файл
    input.close();
}

// запись в файл
void saveToProfile(int n) {
    string Profile;
    if(n==1)  Profile = Profile1;
    else  Profile = Profile2;
    // открываем поток данных для записи в файл
    std::ofstream output(Profile);

    output << tr.p1.pos.x << " " << tr.p1.pos.y << std::endl;
    output << tr.p2.pos.x << " " << tr.p2.pos.y << std::endl;
    output << tr.p3.pos.x << " " << tr.p3.pos.y << std::endl;

    // перебираем точки
    for (auto point: points) {
        // выводим через пробел построчно: x-координату, y-координату и номер множества
        output << point.pos.x << " " << point.pos.y  << std::endl;
    }

    // закрываем
    output.close();
}

void ShowBackgroundSetting() {
    // если не раскрыт список `Background`
    if (!ImGui::CollapsingHeader("Background"))
        // заканчиваем выполнение
        return;

    // Инструмент выбора цвета
    if (ImGui::ColorEdit3("Background color", color)) {
        // код вызывается при изменении значения

        // задаём цвет фона
        setColor(color);
    }
}

// работа с файлами
void ShowFiles() {
    // если не раскрыта панель `Files`
    if (!ImGui::CollapsingHeader("Files"))
        // заканчиваем выполнение
        return;

    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку загрузки
    if (ImGui::Button("Load Profile1")) {
        // загружаем данные из файла
        loadFromProfile(1);
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку сохранения
    if (ImGui::Button("Save Profile1")) {
        // сохраняем задачу в файл
        saveToProfile(1);
    }
    // восстанавливаем буфер id
    ImGui::PopID();
    // третий элемент
    ImGui::PushID(2);
    if (ImGui::Button("Load Profile2")) {
        // загружаем данные из файла
        loadFromProfile(2);
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // четвертый элемент
    ImGui::PushID(3);

    if (ImGui::Button("Save Profile2")) {
        // сохраняем задачу в файл
        saveToProfile(2);
    }
    ImGui::PopID();
}

void ShowClear()
{
    // если не раскрыта панель `Clear`
    if (!ImGui::CollapsingHeader("Clear"))
        // заканчиваем выполнение
        return;
    ImGui::PushID(0);
    if (ImGui::Button("Clear dots")) {
        // очищаем массив точек
        points.clear();
        l.p1=sf::Vector2i(-10,-10);
        l.p2=sf::Vector2i(-10,-10);
    }
    ImGui::PopID();
    ImGui::PushID(1);
    if (ImGui::Button("Clear triangle")) {
        // очищаем треугольник
        tr.p1.pos.x=-10;
        tr.p1.pos.y=-10;
        tr.p2.pos.x=-10;
        tr.p2.pos.y=-10;
        tr.p3.pos.x=-10;
        tr.p3.pos.y=-10;
        tr.i=0;
    }
    ImGui::PopID();
}


/* решение задачи
void solve() {
    // у совпадающих по координатам точек меняем множество на SET_CROSSED
    for (int i = 0; i < points.size(); i++)
        for (int j = i + 1; j < points.size(); j++)
            if (points[i].pos == points[j].pos)
                points[i].setNum = points[j].setNum = SET_CROSSED;

    // у всех точек, у которых множество не SET_CROSSED, задаём множество SET_SINGLE
    for (auto &point: points)
        if (point.setNum != SET_CROSSED)
            point.setNum = SET_SINGLE;

}*/

// рисование задачи на невидимом окне во всё окно приложения
void RenderTask() {
    // задаём левый верхний край невидимого окна
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // задаём правый нижний край невидимого окна
    ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE_X, WINDOW_SIZE_Y));
    // создаём невидимое окно
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    // получаем список примитивов, которые будут нарисованы
    auto pDrawList = ImGui::GetWindowDrawList();

    // перебираем точки из динамического массива точек
    for (auto point: points) {
        ImColor clr;
        clr = ImColor(200, 100, 150);
        // добавляем в список рисования круг
        pDrawList->AddCircleFilled(
                sf::Vector2i(point.pos.x, point.pos.y),
                3,
                clr,
                20
        );
    }
    ImColor clr;
    clr = ImColor(100, 100, 250);
    // добавляем в список рисования круг
    pDrawList->AddCircleFilled(
            tr.p1.pos,
            4,
            clr,
            20
        );
    pDrawList->AddCircleFilled(
            tr.p2.pos,
            4,
            clr,
            20
    );
    pDrawList->AddCircleFilled(
            tr.p3.pos,
            4,
            clr,
            20
    );
    if(tr.p3.pos.x!=-10){
        pDrawList->AddLine(
                tr.p1.pos,
                tr.p2.pos,
                clr,
                1.5f
        );
        pDrawList->AddLine(
                tr.p1.pos,
                tr.p3.pos,
                clr,
                1.5f
        );pDrawList->AddLine(
                tr.p3.pos,
                tr.p2.pos,
                clr,
                1.5f
        );
    }
    clr = ImColor(200, 150, 150);
    if(points.size()>=2) {l.p1=points[0]; l.p2=points[1];}
    pDrawList->AddLine(
                sf::Vector2i( l.p1.pos.x + (int) ((l.p1.pos.x - l.p2.pos.x) * maxDistance),
                              l.p1.pos.y + (int) ((l.p1.pos.y - l.p2.pos.y) * maxDistance)),
                sf::Vector2i( l.p1.pos.x - (int) ((l.p1.pos.x - l.p2.pos.x) * maxDistance),
                              l.p1.pos.y - (int) ((l.p1.pos.y - l.p2.pos.y) * maxDistance)),
                ImColor(200, 150, 100),
                1.5f
                );
    ImGui::End();
}

// панель добавления случайных точек
void ShowRandomize() {
    // если не раскрыта панель `Randomize`
    if (!ImGui::CollapsingHeader("Randomize"))
        // заканчиваем выполнение
        return;

    // первый элемент в строке
    ImGui::PushID(0);

    // Инструмент выбора кол-ва
    if (ImGui::DragInt("Count", lastRandoCntBuf, 0.1, 0, 100)) {

    }
    // восстанавливаем буффер id
    ImGui::PopID();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку добавления
    if (ImGui::Button("Add points")) {
        for(int i=0;i<lastRandoCntBuf[0];i++) points.emplace_back( Point::randomPoint() );
    }
    ImGui::PopID();
    ImGui::PushID(2);
    if (ImGui::Button("Add 3 points of triangle")) {
        tr.p1=Point::randomPoint();
        tr.p2=Point::randomPoint();
        tr.p3=Point::randomPoint();
    }
    ImGui::PopID();
}

// ручное добавление элементов
void ShowAddElem() {
    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Add Elem"))
        // заканчиваем выполнение
        return;


    // Инструмент выбора координаты
    if (ImGui::DragInt2("Coords", lastAddPosBuf, 0.5f, 0, std::min(WINDOW_SIZE_X, WINDOW_SIZE_Y))) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }

    // фиксируем id равный 0 для первого элемента
    ImGui::PushID(0);
    // если нажата кнопка `Set 1`
    if (ImGui::Button("Add point"))
        // добавляем то добавляем в список точку, принадлежащую первому множеству
        points.emplace_back(Point(sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1])));
    // восстанавливаем буфер id
    ImGui::PopID();

    // задаём id, равный одному
    ImGui::PushID(1);
    // если нажата кнопка `Set 2`
    if (ImGui::Button("Add point of triangle")){
        if(tr.i==0) tr.p1.pos=sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]);
        if(tr.i==1) tr.p2.pos=sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]);
        if(tr.i==2) {tr.p2.pos=sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]); tr.i=-1;}
        tr.i++;
    }
    // восстанавливаем буфер id
    ImGui::PopID();

}

/*// решение задачи
void ShowSolve() {
    // если не раскрыта панель `Solve`
    if (!ImGui::CollapsingHeader("Solve"))
        return;
    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку решения
    if (ImGui::Button("Solve")) {
        solve();
    }

    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);

    // создаём кнопку очистки
    if (ImGui::Button("Clear")) {
        // удаляем все точки
        points.clear();
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}*/

// главный метод
int main() {
    // создаём окно для рисования
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Geometry Project 6");
    // задаём частоту перерисовки окна
    window.setFramerateLimit(60);
    // инициализация imgui+sfml
    ImGui::SFML::Init(window);


    // задаём цвет фона
    setColor(color);

    // переменная таймера
    sf::Clock deltaClock;
    // пока окно открыто, запускаем бесконечный цикл
    while (window.isOpen()) {
        // создаём событие sfml
        sf::Event event;
        // пока окно принимает события
        while (window.pollEvent(event)) {
            // отправляем события на обработку sfml
            ImGui::SFML::ProcessEvent(event);

            // если событие - это закрытие окна
            if (event.type == sf::Event::Closed) {
                // закрываем окно
                window.close();
            }

            // если событие - это клик мышью
            if (event.type == sf::Event::MouseButtonPressed) {
                // если мышь не обрабатывается элементами imGui
                if (!ImGui::GetIO().WantCaptureMouse) {
                    // меняем координаты последней добавленной точки
                    lastAddPosBuf[0] = event.mouseButton.x;
                    lastAddPosBuf[1] = event.mouseButton.y;
                    // если левая кнопка мыши
                    if (event.mouseButton.button == sf::Mouse::Button::Left)
                        points.emplace_back(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    else {
                        if(tr.i==0) tr.p1.pos=sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        if(tr.i==1) tr.p2.pos=sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        if(tr.i==2) {tr.p3.pos=sf::Vector2i(event.mouseButton.x, event.mouseButton.y); tr.i=-1;}
                        tr.i++;
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        RenderTask();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.13f, 0.8f));

        ImGui::Begin("Control");

        ShowBackgroundSetting();

        ShowAddElem();

        ShowRandomize();

        ShowClear();

        /*// решение задачи
        ShowSolve();
        */
        ShowFiles();

        // конец рисование окна
        ImGui::End();


        // Возвращаем цвет окна к исходному
        ImGui::PopStyleColor();

        // очищаем окно
        window.clear(bgColor);
        // рисуем по окну средствами imgui+sfml
        ImGui::SFML::Render(window);
        // отображаем изменения на окне
        window.display();
    }

    // завершаем работу imgui+sfml
    ImGui::SFML::Shutdown();

    return 0;
}