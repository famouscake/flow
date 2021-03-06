#include "inc/relabel_to_front.h"

using namespace std;

RelabelToFront::RelabelToFront(const ResidualNetwork &A) : E(ResidualNetwork(A))
{
    this->Source = E.getSource();
    this->Sink = E.getSink();
    this->VertexCount = E.getCount();

    this->PushCount = 0;
    this->RelabelCount = 0;
    this->DischargeCount = 0;

    // Инициализация на списъка от върховете
    this->V = vector<Vertex>(VertexCount);
    for (int i = 0; i < VertexCount; ++i)
    {
        // Инициализация на указател към текущо ребро
        V[i].NCurrent = E.getOutgoingEdges(i).begin();
    }

    // Масив, в които се запазва броя на върховете на всяка от височините
    this->HeightCount = vector<int>(2 * VertexCount, 0);

    // Източника и шахтата имат статична височина
    this->HeightCount[VertexCount] = 1;
    this->HeightCount[0] = VertexCount - 1;

    V[Source].Height = VertexCount;
}

RelabelToFront::~RelabelToFront()
{
}

void RelabelToFront::Run()
{
    // Изпълнението на евристиката Global Relabeling Heuristic
    SetInitialLabels();

    // Избутване на поток равен на разреза ({s}, V\{s})
    for (auto &edge : E.getOutgoingEdges(Source))
    {
        if (edge.weight > 0)
        {
            V[Source].ExcessFlow += edge.weight;
            Push(edge);
        }
    }

    // Основно действие на алгоритъма
    do
    {
        int i = ActiveQueue.front();
        ActiveQueue.pop();

        Discharge(i);

    } while (!ActiveQueue.empty());
}

void RelabelToFront::Discharge(const int i)
{
    this->DischargeCount++;

    auto begin = E.getOutgoingEdges(i).begin();
    auto end = E.getOutgoingEdges(i).end();
    auto current = V[i].NCurrent;

    while (V[i].ExcessFlow > 0)
    {
        if (current == end)
        {
            int oldHeight = V[i].Height;

            Relabel(i);

            // Изпълнение на евристиката Gap Heuristic, ако е намерена празна височина
            if (HeightCount[oldHeight] == 0)
            {
                Gap(oldHeight);
            }

            current = begin;
            continue;
        }
        else if (CanPush(*current))
        {
            Push(*current);
        }

        current++;
    }

    V[i].NCurrent = current;
}

void RelabelToFront::Push(ResidualEdge &edge)
{
    this->PushCount++;

    // Ако избутваме поток към неактивен връх, се добавя към опашката
    if (V[edge.to].ExcessFlow == 0 && edge.to != Source && edge.to != Sink)
    {
        ActiveQueue.push(edge.to);
    }

    int min = std::min(V[edge.from].ExcessFlow, edge.weight);

    edge.weight -= min;
    E.E[edge.to][edge.index].weight += min;

    V[edge.from].ExcessFlow -= min;
    V[edge.to].ExcessFlow += min;
}

void RelabelToFront::Relabel(const int i)
{
    this->RelabelCount++;
    this->V[i].RelabelCount++;

    auto minHeight = 2 * VertexCount;
    for (const auto &edge : E.getOutgoingEdges(i))
    {
        if (edge.weight > 0)
        {
            minHeight = min(minHeight, V[edge.to].Height);
        }
    }

    HeightCount[V[i].Height]--;
    V[i].Height = minHeight + 1;
    HeightCount[V[i].Height]++;
}

void RelabelToFront::Gap(const int k)
{
    // Промяна на височините на всички върхове ако е намерена дупка във височините
    for (int i = 0; i < VertexCount; i++)
    {
        if (i != Source && i != Sink && V[i].Height >= k)
        {
            HeightCount[V[i].Height]--;
            V[i].Height = std::max(V[i].Height, VertexCount + 1);
            HeightCount[V[i].Height]++;
        }
    }
}

bool RelabelToFront::CanPush(const ResidualEdge &edge)
{
    // 1) Must be overflowing
    if (!IsOverflowing(edge.from))
    {
        return false;
    }

    // 2) There must exist an edge in the residual network
    if (edge.weight == 0)
    {
        return false;
    }

    // 4) i.h = j.h +1
    if (V[edge.from].Height != V[edge.to].Height + 1)
    {
        return false;
    }

    return true;
}

bool RelabelToFront::CanRelabel(const int i)
{
    // 1) Must be overflowing
    if (!IsOverflowing(i))
    {
        return false;
    }

    // 2) All neigbors must be highter
    for (auto edge : E.getOutgoingEdges(i))
    {
        if (edge.weight > 0)
        {
            if (V[edge.from].Height > V[edge.to].Height)
            {
                return false;
            }
        }
    }

    return true;
}

bool RelabelToFront::IsOverflowing(const int i)
{
    return V[i].ExcessFlow > 0 && i != Source && i != Sink;
}

// Изпълнява обратно търсене в дълбочина
void RelabelToFront::SetInitialLabels()
{
    queue<int> q;
    vector<bool> A = vector<bool>(VertexCount, false);

    q.push(Sink);
    A[Sink] = true;
    A[Source] = true;

    while (!q.empty())
    {
        const int u = q.front();
        q.pop();

        const int dist = V[u].Height + 1;

        for (const auto &edge : E.getOutgoingEdges(u))
        {
            if (E.E[edge.to][edge.index].weight > 0 && A[edge.to] == false)
            {
                q.push(edge.to);
                A[edge.to] = true;

                HeightCount[V[edge.to].Height]--;
                V[edge.to].Height = dist;
                HeightCount[V[edge.to].Height]++;
            }
        }
    }
}
