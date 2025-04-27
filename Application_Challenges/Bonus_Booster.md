# Bistro 92 RESTful API and Scalability Solution

This project outlines a **RESTful API** for Bistro 92’s smart pad ordering system, designed for high-concurrency order placement, and a proposed **cloud-native architecture** to ensure extreme scalability for thousands of simultaneous orders without data loss. The solution addresses the Bonus Boosters challenges (Q1 and Q2) for the Bistro 92 Techathon.

## Q1: RESTful API for Order Placement

The API, built with **Express.js** (deployed on Vercel) and **PostgreSQL** (deployed on Render), supports efficient order placement from smart pads, handling high concurrency with robust error management.

### Endpoints

- **GET /menu_items**
  - **Purpose**: Retrieves the menu for smart pads.
  - **Request**: `GET https://techathon-server-main.vercel.app/menu_items`
  - **Response (200)**: Array of menu items (e.g., `[{"id": "m1", "name": "Grilled Salmon", "price": 18.99}, ...]`)
  - **Error (500)**: `{"error": "Fetch failed"}`
- **POST /orders**
  - **Purpose**: Submits orders from smart pads.
  - **Request**: `POST https://techathon-server-main.vercel.app/orders`
  - **Headers**: `Authorization: Bearer <table_token>`
  - **Body**: `{"table_number": "T12", "items": [{"menu_item_id": "m1", "quantity": 2}], "version": 5, "order_id": "id-1234"}`
  - **Response (201)**: `{"order_id": "o123", "status": "received"}`
  - **Errors**:
    - 400: `{"error": "Invalid item"}`
    - 409: `{"error": "Version conflict"}`
    - 429: `{"error": "Rate limit"}`

### Concurrency Controls

- **Atomic Transactions**: PostgreSQL ensures consistent order writes.
- **Optimistic Locking**: `version` field prevents conflicts (returns 409).
- **Redis Queue**: Processes 1,000+ requests sequentially.
- **Rate Limiting**: 5 requests/min per table via Redis (returns 429).
- **Idempotency**: `order_id` prevents duplicate orders.
- **Circuit Breaker**: Mitigates downstream failures.

## Q2: Enhanced API Scalability

A proposed **cloud-native architecture** ensures the API can handle thousands of simultaneous orders with zero data loss, designed for future scalability.

### Strategies

- **Microservices & Kubernetes**:
  - Splits API into microservices (e.g., order, payment), scaled via Kubernetes (e.g., 10 pods at 70% CPU during peak).
  - Benefit: Fault-tolerant, efficient scaling.
- **Kafka Processing**:
  - Queues orders in Kafka, partitioned by table, handling 10,000+ orders/min.
  - Benefit: Reliable processing for 5,000+ queued orders.
- **Cassandra Database**:
  - Uses sharded Cassandra for high-speed writes (e.g., 5ms for order writes, global replication).
  - Benefit: Resilient, fast data storage.
- **Load Balancing**:
  - AWS Global Accelerator routes 10,000 requests/sec with 80ms response time.
  - Benefit: Low latency, dynamic resource allocation.
- **Redis Caching**:
  - Caches menu in Redis, reducing database load by 80% for 1M requests/sec.
  - Benefit: Faster responses with Kafka-driven cache invalidation.
- **Idempotency**:
  - Unique IDs and Saga pattern deduplicate retried orders.
  - Benefit: Consistent, duplicate-free data.
- **Monitoring**:
  - Prometheus/Grafana and Chaos Mesh detect and reroute failures in <10s.
  - Benefit: Proactive issue resolution.
- **Rate-Limiting**:
  - Adaptive limits and circuit breakers throttle spam (e.g., 100/min returns 429).
  - Benefit: System protection under high load.

## Benefits

- **Reliability**: Ensures no data loss for thousands of orders.
- **Scalability**: Handles millions of daily orders with low latency.
- **Efficiency**: Reduces database load and optimizes resource use.
- **Robustness**: Proactively mitigates failures and spam.

## Conclusion

The RESTful API and proposed scalability architecture position Bistro 92 to handle high-concurrency order placement and future growth seamlessly. The solution leverages modern technologies to ensure reliability, performance, and a superior dining experience.