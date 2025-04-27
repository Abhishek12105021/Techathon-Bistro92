# Tech Tricks (450 Points)

## Q1 (70 points):  
**Design a database schema for Bistro 92 to track users, orders, menu items, tables, and payments, optimized for fast queries.**

---

## Short Schema Overview

The database for Bistro 92 is designed to be highly normalized and optimized for fast querying during restaurant operations like placing orders, updating menus, and tracking payments. 

Here’s a breakdown of the main tables:

- **tables**  
  Each physical restaurant table is stored with a unique `table_id` and `table_number`.
  
- **menu_items**  
  Stores all menu dishes with their names and prices. Supports flexible menu management and fast item lookup.
  
- **orders**  
  Each order is linked to a table. It includes the order status (e.g., pending, confirmed) and timestamps to monitor order processing speed.
  
- **order_items**  
  This junction table captures which menu items belong to which orders, along with the quantity ordered. Helps in detailed item tracking per order.

The structure is lightweight and efficient for high-frequency operations, using simple `INTEGER` foreign keys and indexed primary keys (via `SERIAL`). Timestamps help with real-time monitoring.

---
### [👉 SQL Schema Code](https://github.com/MehemudAzad/techathon_server/blob/main/src/model/database.sql)

---

## Q2 (80 points):  
**Write an SQL query to retrieve all orders from the last hour, including table number, items ordered, and order time, optimized for speed.**

---

## Query Overview

The goal is to efficiently fetch all orders placed within the last hour, along with essential order details like table number, ordered items, and their quantities.

The query makes use of optimized joins and time-based filtering to ensure minimal database load even during peak hours.

---

## SQL Query

```sql
SELECT 
    o.order_id,
    t.table_number,
    o.order_time,
    o.status,
    mi.name AS item_name,
    oi.quantity
FROM orders o
JOIN tables t ON o.table_id = t.table_id
LEFT JOIN order_items oi ON o.order_id = oi.order_id
LEFT JOIN menu_items mi ON oi.item_id = mi.item_id
WHERE o.order_time >= NOW() - INTERVAL '1 hour'
ORDER BY o.order_time DESC;
```

---
## Q3 (90 points):  
**Implement a feature to notify kitchen staff in real-time when a new order is placed, and describe your tech stack**

---
- [👉 View Kitchen Dashboard Demo](https://stellular-sopapillas-a959dd.netlify.app/orders)  

## Tech Stack Used
- **Backend:** Node.js, Express.js
- **Database:** PostgreSQL
- **Hosting:** Vercel (Serverless Functions)
- **Simulation Integration:** Wokwi API-based hardware simulation
- **Real-Time Communication:** Server-Sent Events (SSE)

---
## Q4 (100 points):
Describe a cloud-based system architecture for real-time updates, data storage, and smart pad communication, ensuring low latency and high availability.

---

## Description
This architecture enables real-time updates, efficient data storage, and seamless communication with smart pads for Bistro 92's smart ordering system. By leveraging cloud technologies, this solution ensures low latency, high availability, and scalability.

---

## Architecture Components

### 1. **Cloud Provider:**
   - **AWS** is used for scalability and reliability. AWS offers various tools to meet the demands of high-volume operations like Bistro 92, ensuring that the system can scale up or down depending on traffic and usage.

### 2. **Frontend:**
   - Smart pads running **React apps** communicate with the backend via **WebSocket** for real-time updates. The React app enables the kitchen staff and waiters to view and interact with orders as they are placed.

### 3. **Backend:**
   - The **Node.js** server with **Express.js** powers the backend, hosted on **AWS Elastic Beanstalk** for auto-scaling. Elastic Beanstalk handles the deployment, scaling, and monitoring of the application in the cloud.

### 4. **Database:**
   - **PostgreSQL** is used on **AWS RDS** (Relational Database Service) for data storage. It includes read replicas for improving query performance, ensuring that orders and menu items can be accessed quickly, even during peak traffic.

### 5. **Real-Time Messaging:**
   - **AWS API Gateway** with **WebSocket** enables low-latency communication between the smart pads and the backend. When an order is placed, a real-time update is pushed to the relevant smart pad.

### 6. **Caching:**
   - **Redis** is used via **AWS ElastiCache** to cache frequently accessed data such as menu items. This reduces the load on the PostgreSQL database and improves response time.

### 7. **Load Balancing:**
   - **AWS Application Load Balancer** ensures high availability by distributing incoming traffic across multiple instances of the backend application, preventing any one instance from becoming overloaded.

### 8. **Monitoring:**
   - **AWS CloudWatch** is utilized for tracking the performance of the application, detecting failovers, and ensuring that the system is running smoothly at all times.

---

## Key Features

### 1. **Low Latency:**
   - **WebSocket** for real-time communication between smart pads and backend, coupled with **Redis caching**, ensures response times below **100ms**, enabling quick updates to the smart pads.

### 2. **High Availability:**
   - The system ensures high availability with **Multi-AZ PostgreSQL** for fault tolerance, and **Elastic Beanstalk** auto-scaling that adapts to traffic demands, minimizing downtime.

### 3. **Scalability:**
   - The architecture is designed to scale efficiently to handle over **1,000 simultaneous orders** by distributing load via **AWS Application Load Balancer** and using **read replicas** for efficient database queries.

---

## Q5 (110 points):
Design a real-time dashboard showing pending orders, average fulfillment time, and total sales, specifying tools and justifying your choices.

---

## Description
This dashboard is designed to display real-time operational metrics for Bistro 92, including pending orders, average fulfillment time, and total sales. The dashboard will enhance operational efficiency by providing up-to-date insights into the restaurant's performance.

---

## Design and Tools

### 1. **Frontend:**
   - The dashboard is built with **React** for a dynamic user interface, using **Chart.js** to display interactive visualizations of key metrics. The frontend is hosted on **Vercel** for fast deployment and scalability, ensuring smooth and quick updates.

### 2. **Backend:**
   - The backend is powered by **Node.js** with **Express.js**, which handles requests and fetches data from the **PostgreSQL** database via **REST APIs**. The backend performs data calculations such as average fulfillment time and total sales.

### 3. **Database:**
   - **PostgreSQL** is used for storing order and sales data, with **indexes** on frequently queried fields to ensure fast response times when retrieving the necessary data for the dashboard.

### 4. **Real-Time Updates:**
   - **WebSocket** (via **Socket.IO**) is used to provide real-time updates to the dashboard. As orders are updated or completed, the frontend is updated instantaneously to reflect changes in pending orders, average fulfillment time, and total sales.

---

## Metrics

### 1. **Pending Orders:**
   - Displays the count of orders with a status of `'pending'`.

### 2. **Average Fulfillment Time:**
   - The average time taken to fulfill orders, calculated as:
     ```sql
     AVG(completion_time - order_time)
     ```
   - Only completed orders are considered in the calculation.

### 3. **Total Sales:**
   - The total sales amount is calculated as:
     ```sql
     SUM(oi.quantity * mi.price)
     ```
   - This sum aggregates the sales for all items ordered in completed orders.

---

## Monitoring

### 1. **Prometheus & Grafana:**
   - **Prometheus** is used for performance tracking, collecting and storing metrics from the backend.
   - **Grafana** is integrated with Prometheus to provide rich, visual dashboards for system health, performance, and operational metrics.

---

## Justification for Choices

### 1. **React/Chart.js:**
   - **React** provides a lightweight and efficient framework for building the dynamic dashboard.
   - **Chart.js** is a popular choice for building interactive charts with minimal configuration, making it perfect for real-time data visualization.

### 2. **WebSocket:**
   - Using **WebSocket** ensures that updates are sent instantly to the frontend without the need for constant polling, resulting in a more responsive and efficient dashboard.

### 3. **PostgreSQL:**
   - **PostgreSQL** is a reliable relational database system for complex queries and is well-suited for handling the transactional data of orders and sales, especially with **indexed performance** for speed.

### 4. **Prometheus/Grafana:**
   - **Prometheus** and **Grafana** provide powerful and flexible monitoring solutions for tracking system health and performance, enabling quick identification of issues and trends.

---


