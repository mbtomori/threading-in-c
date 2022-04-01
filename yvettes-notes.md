Yvette’s Notes:
If two people meet in the middle of stairs, then no one can move and deadlock results.   Adding a flag could control the direction of flow (up or down).  
Need a counting semaphore for number of stairs (I think???)
If the direction of flow is opposite from the direction of a waiting customer, then no new additional customers should be added in the same direction of current flow because a continuous stream of customers going in one direction on the stairs would prevent other customers from going in the opposite direction, resulting in unfairness and/or starvation. 