#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int id;
	unsigned blocked : 1;
} thread;

typedef struct queue_node {
	thread *thread;
	struct queue_node *next;
} queue_node;

typedef struct {
	queue_node *head;
	queue_node *tail;
} queue;

typedef struct {
	int timeslice;
	queue *queue;
} scheduler;

static queue *create_queue(void);

static void start_timer_sync(void);
int current_thread();

static queue_node *new_node(thread*);
static void free_node(queue_node*);
static thread *make_thread(int id);

static void enqueue(queue*, queue_node*);
static queue_node *dequeue(queue*);
static queue_node *dequeue_non_blocked_thread(queue *q);

static scheduler sched = {};
static int curr_thread_id = -1;

/*
 * Setups the scheduler quantum of time.
 */
void scheduler_setup(int timeslice)
{
	printf("scheduler_setup %d\n", timeslice);

	sched.timeslice = timeslice;
	sched.queue = create_queue();
}

/*
 * Creates a new thread with @thread_id and appends it to the end of the queue.
 */
void new_thread(int thread_id)
{
	printf("new_thread %d\n", thread_id);

	queue_node *node = new_node(make_thread(thread_id));
	enqueue(sched.queue, node);
}

/*
 * Notifies the scheduler that the thread won't no long be executed.
 */
void exit_thread()
{
	printf("exit_thread %d\n", current_thread());

	queue_node *node = dequeue(sched.queue);
	if (!node)
		return;

	curr_thread_id = -1;
	free_node(node);

	node = dequeue_non_blocked_thread(sched.queue);
	if (!node)
		return;

	curr_thread_id = node->thread->id;
	enqueue(sched.queue, node);
}

static queue_node *dequeue_non_blocked_thread(queue *q)
{
	queue_node *it = q->head;
	if (!it)
		return NULL;

	while (it) {
		printf("thread_id: %d\n", it->thread->id);
		it = it->next;
	}

	return NULL;
}

/*
 * Notifies the scheduler that the current thread should be blocked because of
 * some long operations, e.g IO.
 */
void block_thread()
{
	printf("block_thread %d\n", current_thread());
}

/*
 * Notifies scheduler that thread with the @thread_id is ready. The thread
 * is put at the end of the queue.
 */
void wake_thread(int thread_id)
{
	printf("wake_thread %d\n", thread_id);
}

/*
 * Timer handler.
 */
void timer_tick()
{
	static int tick = 0;
	if (tick++ % sched.timeslice == 0) {
		// switch the thread
	}
	printf("timer_tick: %d\n", tick);
}

/*
 * Returns the descriptor of currently executed thread.
 */
int current_thread()
{
	return curr_thread_id;
}

static inline queue *create_queue(void)
{
	return calloc(1, sizeof(queue));
}

static queue_node *new_node(thread* t)
{
	queue_node *ret = calloc(1, sizeof(queue_node));
	if (!ret) {
		fprintf(stderr, "calloc (new_node), thread_id=%d\n", t->id);
		exit(1);
	}

	ret->next = NULL;
	ret->thread = t;

	return ret;
}

static void enqueue(queue *q, queue_node *node)
{
	if (!q->tail) {
		q->head = q->tail = node;
		return;
	}

	q->tail->next = node;
	q->tail = node;
}

static queue_node *dequeue(queue *q)
{
	if (!q->head)
		return NULL;

	queue_node *head = q->head;
	queue_node *new_head = head->next;

	if (new_head) {
		q->head = new_head;
		head->next = NULL;
	} else {
		q->head = q->tail = NULL;
	}

	return head;
}

static thread *make_thread(int id)
{
	thread *ret = (thread *)calloc(1, sizeof(thread));
	if (!ret) {
		fprintf(stderr, "calloc (new_thread), id=%d\n", id);
		exit(1);
	}

	ret->id = id;
	return ret;
}

static void free_node(queue_node *node)
{
	free(node->thread);
	free(node);
}

static void start_timer_sync(void)
{
	const int it = 1;
	for (int n = 0; n < it; n++) {
		new_thread(n);
		timer_tick();
	}
	dequeue_non_blocked_thread(sched.queue);
}

int main()
{
	const int timeslice = 1;

	scheduler_setup(timeslice);
	start_timer_sync();

	free(sched.queue);

	return 0;
}
