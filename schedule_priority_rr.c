#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *headNode = NULL;
struct node *last = NULL;
struct node *newTask = NULL;
struct node *currentNode = NULL;

void add(char *name, int priority, int burst)
{
    newTask = malloc(sizeof(struct node));
    newTask->task = malloc(sizeof(struct task));
    newTask->task->name = name;
    newTask->task->priority = priority;
    newTask->task->burst = burst;

    // if headNode is null, create a newTask node and set it to headNode
    if (headNode == NULL)
    {
        // create a newTask node
        headNode = newTask;
        headNode->next = NULL;
    }
    else
    {
        currentNode = headNode;
        // if headNode priority is less than newTask priority
        if (headNode->task->priority < priority)
        {
            newTask->next = headNode;
            headNode = newTask;
        }
        else
        {
            // if headNode priority is more than newTask priority
            // find the node that has a lower priority than newTask
            while (currentNode->next != NULL && currentNode->next->task->priority >= priority)
            {
                currentNode = currentNode->next;
            }
            newTask->next = currentNode->next;
            currentNode->next = newTask;
        }
    }
}


// invoke the scheduler
void schedule()
{
    int burst = 0;
    currentNode = headNode;
    struct node *ref = headNode;
    int time_slice = 0;
    int priority = MIN_PRIORITY;

    while (headNode != NULL)
    {
        int processed_tasks_with_current_priority = 0;

        // Find the next task with the highest priority
        while (ref != NULL)
        {
            if (ref->task->priority < priority)
            {
                ref = ref->next;
                continue;
            }

            time_slice = (ref->task->burst >= QUANTUM) ? QUANTUM : ref->task->burst;
            run(ref->task, time_slice);
            burst += time_slice;
            printf("Time is now: %d time units\n", burst);
            ref->task->burst -= time_slice;

            if (ref->task->burst == 0)
            {
                // Task is complete, remove it from the list
                struct node *temp = ref;
                ref = ref->next;
                delete (&headNode, temp->task);
                free(temp->task);
                free(temp);
            }
            else
            {
                // Task is not complete, move to the next task
                processed_tasks_with_current_priority++;
                ref = ref->next;
            }

            // If all tasks have been processed, start over
            if (ref == NULL)
            {
                ref = headNode;
            }
        }

        if (processed_tasks_with_current_priority == 0)
        {
            // No tasks with the current priority, move to the next priority
            priority++;
        }
    }
}