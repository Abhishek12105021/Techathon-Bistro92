# Bistro 92 System: Quick Fixes (150 Points)

---

## **Q1 (25 points): List three essential features Bistro 92’s system needs for customer satisfaction and efficient order processing.**

### Answer:

### 1. Intuitive and Reliable User Interface:
- **Minimal User Input:** Use a touchscreen panel to allow easy menu browsing and ordering with the fewest possible taps. Avoid unnecessary confirmation steps that could complicate the customer experience.
- **Local Order Storage:** Equip the ESP32 with onboard memory to temporarily store orders during internet or power failures, ensuring that no customer orders are lost.

### 2. Enriched Menu and Backend Database:
- Maintain a detailed database including food names, prices, ingredients, and descriptions. This enriches customer decision-making and supports smoother backend processing.

### 3. Intelligent and Optimized Kitchen Order Management:
- Implement efficient server-side algorithms to prioritize meal preparation based on dish complexity, customer wait times, and table grouping, ensuring faster service and better synchronization of meal delivery.

---

## **Q2 (25 points): Describe two design principles to make the smart pad interface intuitive for all users, including tech novices.**

### Answer:

### 1. Simplicity:
- Design the interface with a clean, uncluttered layout featuring large, clearly labeled buttons.
- Keep the number of steps minimal to complete actions (e.g., press Button 1 to access the menu, Button 2 to confirm selections), reducing confusion especially for tech novices.

### 2. Effective Feedback Mechanisms:
- Provide immediate visual feedback, such as highlighting selected items on the OLED display.
- Optionally include simple auditory cues (like a soft beep) to confirm actions.
- This reassures users that their input has been received, making the system more accessible for elderly or visually impaired customers.

---

## **Q3 (30 points): Identify three potential security vulnerabilities in Bistro 92’s system (e.g., theft, order tampering) and suggest one solution for each.**

### Answer:

### 1. Vulnerability: Order Tampering
- **Description:** Unauthorized users could modify orders in transit to the cloud, leading to incorrect items being prepared.
- **Solution:** Implement end-to-end encryption for data transmission between the smart pad and the cloud to ensure orders cannot be altered by interceptors.

### 2. Vulnerability: Device Theft
- **Description:** Smart pads placed on tables could be stolen, compromising the system and incurring replacement costs.
- **Solution:** Secure devices to tables with tamper-proof mounts or locks, and include a GPS tracking feature to locate stolen devices.

### 3. Vulnerability: Unauthorized Access to Admin Dashboard
- **Description:** Hackers could gain access to the admin dashboard, exposing sensitive order and sales data.
- **Solution:** Use multi-factor authentication (MFA) for dashboard access, requiring a password and a secondary verification method (e.g., a mobile app code).

---

## **Q4 (30 points): Explain two strategies to keep Bistro 92’s system responsive and stable during peak hours.**

### Answer:

### 1. Load Balancing:
- Deploy multiple cloud servers with a load balancer to distribute incoming order requests evenly across servers.
- This prevents any single server from becoming overwhelmed during peak hours, ensuring low latency and system stability.

### 2. Caching Frequently Accessed Data:
- Store frequently accessed data, such as the menu and popular item details, in a fast in-memory cache (e.g., Redis) on both the smart pads and the cloud system.
- This reduces database queries and speeds up response times during high traffic.

As mentioned earlier, we need to ensure that the ESP32/order panel has an HDD memory to save orders during power drops.  
However, for caching, we require faster memory, such as SSD, to process the cache efficiently.  
Given that adding more storage would be expensive for the touch panel, we could use virtual memory with TLB (Translation Lookaside Buffer) to ensure efficient use of available memory.  
By implementing multiple levels of caching, we can access data faster while keeping costs manageable.

---

## **Q5 (40 points): Describe one method to integrate the existing inventory system with Bistro 92’s new system without disrupting operations.**

### Answer:

### Method: API-Based Integration with Manager Authentication and Database Triggers

### Description:
Integrate Bistro 92’s new smart pad ordering system with the existing inventory system using a RESTful API, restricted to manager authentication for inventory updates, and database triggers for automatic, real-time inventory adjustments.  
This ensures seamless operation during the transition.

### Steps:

1. **API Development:**
   - Create a RESTful API to link the ordering system with the inventory system.
   - Endpoints allow order data (e.g., items and quantities) to be sent to the inventory system.

2. **Manager Authentication:**
   - Implement role-based authentication (e.g., OAuth2) requiring manager credentials (username/password or token) to authorize inventory updates.
   - This ensures only the manager can modify stock levels.

3. **Database Triggers:**
   - Set up database triggers in the inventory system to automatically deduct ingredient quantities from stock when an order is confirmed in the ordering system’s database.
   - For example, a trigger fires after an order insert to update the inventory table.

4. **Real-Time Admin Updates:**
   - Use WebSocket or Server-Sent Events to push inventory and order updates to the admin dashboard in real time, reflecting changes instantly for staff monitoring.

5. **Parallel Transition:**
   - Run the new system alongside the existing one initially, testing on a few tables.
   - Managers verify inventory sync accuracy via the authenticated API.
   - Gradually scale to all tables, ensuring no operational downtime.

---

