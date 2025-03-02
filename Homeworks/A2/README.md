# DNS Resolver (Iterative & Recursive Lookup)

## Overview

This project implements a **DNS Resolver** that supports both **Iterative** and **Recursive** DNS lookups using Python and the `dnspython` library. The iterative lookup follows the standard DNS resolution hierarchy (Root → TLD → Authoritative), while the recursive lookup utilizes the system's DNS resolver to fetch the final result.

## Team Members

| **Name**          | **Roll Number**     | **Email**          |
| ----------------- | ------------------- | ------------------ |
| Venkatesh Akula      | 220109     | akulav22@iitk.ac.in      |
| Mohammed Anas  | 220654 | mohdanas22@iitk.ac.in |
| Sai Nikhil  | 221095 | ssain22@iitk.ac.in |

## Implemented Features

- **Iterative DNS Resolution**: Manually queries Root, TLD, and Authoritative servers to resolve a domain.
- **Recursive DNS Resolution**: Uses the system's DNS resolver to fetch the final IP.
- **Error Handling**: Handles timeout errors, unreachable servers, and invalid domains.
- **Command-line Interface**: Supports execution via the terminal with user-specified parameters.


## Not Implemented Features

- Handling record types other than A and NS.
- Caching of previously resolved queries.
  
---

## Installation

### Prerequisites

Ensure that Python (3.x) is installed on your system. You also need to install the `dnspython` library:

```sh
pip install dnspython
```

---

## Usage

### Running the DNS Resolver

The script accepts two arguments:

1. **Mode** (`iterative` or `recursive`)
2. **Domain Name** (e.g., `google.com`)

```sh
python3 dns_resolver.py <mode> <domain name>
```


#### **Example 1: Iterative DNS Resolution**

```sh
python3 dns_resolver.py iterative google.com
```

##### **Expected Output:**

```
[Iterative DNS Lookup] Resolving google.com
[DEBUG] Querying ROOT server (198.41.0.4) - SUCCESS
Extracted NS hostname: l.gtld-servers.net.
Resolved l.gtld-servers.net. to 192.41.162.30
[DEBUG] Querying TLD server (192.41.162.30) - SUCCESS
Extracted NS hostname: ns1.google.com.
Resolved ns1.google.com. to 216.239.32.10
[DEBUG] Querying AUTH server (216.239.32.10) - SUCCESS
[SUCCESS] google.com -> 142.250.194.78
Time taken: 0.650 seconds
```

#### **Example 2: Recursive DNS Resolution**

```sh
python3 dns_resolver.py recursive google.com
```

##### **Expected Output:**

```
[Recursive DNS Lookup] Resolving google.com
[SUCCESS] google.com -> ns4.google.com.
[SUCCESS] google.com -> ns3.google.com.
[SUCCESS] google.com -> ns2.google.com.
[SUCCESS] google.com -> ns1.google.com.
[SUCCESS] google.com -> 172.217.167.206
Time taken: 0.014 seconds
```

---

## Code Explanation

### 1. Send_dns_query

- Sends a DNS query to a given nameserver for an **A record**.
- Uses **UDP** for communication with a timeout mechanism.
- Returns the DNS response or `None` in case of failure.

### 2. Extract_next_nameservers

- Extracts **NS (nameserver) records** from the authority section.
- Resolves the extracted NS hostnames into **IP addresses**.

### 3. Iterative_dns_lookup

- **Starts at Root DNS servers** and iteratively queries the next set of servers.
- **Moves from Root → TLD → Authoritative** servers to resolve the domain.
- **Stops when an answer is found** or if resolution fails.

### 4. Recursive_dns_lookup

- **Directly queries the system's DNS resolver** to fetch results.
- **First fetches NS records**, then resolves A records.
- **Handles exceptions like timeouts and invalid domains.**

### 5. Execution

- **Parses command-line arguments** (`iterative` or `recursive`).
- Calls the appropriate function and **measures execution time**.

---

## Error Handling

- **Timeout Handling:**  If a server does not respond within 3 seconds, the query fails gracefully.
- **Invalid Domain Handling:** Catches exceptions when resolving a non-existent domain.
- **Unreachable Server Handling:** Detects when a nameserver cannot be contacted.
- **Recursuive Lookup Failure** Catches the exception and prints an error message.

---

## **Testing**

### Correctness Testing

- Verified iterative resolution using various domains (e.g., `google.com`,`amazon.com`,`abc.xyz` ).
- Checked error handling for invalid and unreachable domains.

## Team Contribution
- Venkatesh (220109) [**`33.33%`**]
- Sai Nikhil (221095) [**`33.33%`**]
- Anas (220654) [**`33.33%`**]

All three of us contributed equally to the assignment and README.

## **Declaration**

We hereby declare that our code is original and does not involve plagiarism. The assignment was completed following the academic integrity guidelines.

## **Feedback**

This assignment gave us a hands-on learning experience in understanding and implementing DNS resolution mechanisms!
We would like to express our gratitude to the Instructor and TAs for their continuous support throughout the assignment.

### _Thank You!_
---
