#include "Event.hpp"
#include <algorithm>

void EventQueue::push(const Event &e)
{
    queue.push(e);
}

Event EventQueue::pop()
{
    Event top = queue.top();
    queue.pop();
    return top;
}

bool EventQueue::empty() const
{
    return queue.empty();
}
// void EventQueue::heapifyUp(int index) {
//     while (index > 0) {
//         int parent = (index - 1) / 2;
//         if (queue[index].time >= queue[parent].time) break;
//         std::swap(queue[index], queue[parent]);
//         index = parent;
//     }
// }

// void EventQueue::heapifyDown(int index) {
//     int smallest = index;
//     int left = 2 * index + 1;
//     int right = 2 * index + 2;

//     if (left < (int)queue.size() && queue[left].time < queue[smallest].time)
//         smallest = left;
//     if (right < (int)queue.size() && queue[right].time < queue[smallest].time)
//         smallest = right;

//     if (smallest != index) {
//         std::swap(queue[index], queue[smallest]);
//         heapifyDown(smallest);
//     }
// }

// void EventQueue::push(Event e) {
//     queue.push_back(e);
//     heapifyUp(queue.size() - 1);
// }

// Event EventQueue::pop() {
//     if (queue.empty()) throw std::out_of_range("Pop from empty queue");
//     Event top = queue[0];
//     queue[0] = queue.back();
//     queue.pop_back();
//     heapifyDown(0);
//     return top;
// }

// bool EventQueue::empty() const {
//     return queue.empty();
// }

// size_t EventQueue::size() const {
//     return queue.size();
// }#include "Event.hpp"
