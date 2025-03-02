import dns.message
import dns.query
import dns.rdatatype
import dns.resolver
import time

# Root DNS servers to start the iterative resolution process
ROOT_SERVERS = {
    "198.41.0.4": "Root (a.root-servers.net)",
    "199.9.14.201": "Root (b.root-servers.net)",
    "192.33.4.12": "Root (c.root-servers.net)",
    "199.7.91.13": "Root (d.root-servers.net)",
    "192.203.230.10": "Root (e.root-servers.net)"
}

TIMEOUT = 3  # Timeout in seconds for each DNS query attempt

def send_dns_query(server, domain):
    """ 
    Sends a DNS query to the given server for an A record of the specified domain.
    Returns the response if successful, otherwise returns None.
    """
    try:
        query = dns.message.make_query(domain, dns.rdatatype.A)  # Construct the DNS query
         # TODO: Send the query using UDP 
        # Note that above TODO can be just a return statement with the UDP query!
##########-------TODO-------##########
        response = dns.query.udp(query, server, timeout=TIMEOUT)  # Send query via UDP
        return response  # Return the DNS response
##########-------TODO-------##########
    except Exception as e:
        # print(f"[ERROR] Query failed for {server}: {e}")
        return None # If an error occurs (timeout, unreachable server, etc.), return None

def extract_next_nameservers(response):
    """Extracts nameservers from the authority section and resolves their IPs."""
    ns_ips = []
    ns_names = []
    
   
    # Extract NS records from the authority section
    for rrset in response.authority:
        if rrset.rdtype == dns.rdatatype.NS:
            for rr in rrset:
                ns_name = rr.to_text()
                ns_names.append(ns_name) # Extract nameserver hostname
                print(f"Extracted NS hostname: {ns_name}")
##########-------TODO-------##########
    # Resolve NS names to IP addresses
    for ns_name in ns_names:
        try:
            answer = dns.resolver.resolve(ns_name, "A")  # Get IP of the NS
            for rdata in answer:
                ns_ips.append(rdata.to_text())   # Store all resolved IPs
                print(f"Resolved {ns_name} to {rdata.to_text()}")  # Print the resolved message
        except Exception as e:
            print(f"[WARNING] Failed to resolve {ns_name}: {e}")  #  Error handling
##########-------TODO-------##########
    return ns_ips  # Return list of resolved nameserver IPs

def iterative_dns_lookup(domain):
    """Performs an iterative DNS resolution from root servers to authoritative servers."""
    print(f"[Iterative DNS Lookup] Resolving {domain}")

    next_ns_list = list(ROOT_SERVERS.keys())  # Start with root servers
    stage = "ROOT"

    while next_ns_list:
        ns_ip = next_ns_list.pop(0)  # Pick the first available nameserver
        response = send_dns_query(ns_ip, domain)

        if response: #checks if response is not NONE
            print(f"[DEBUG] Querying {stage} server ({ns_ip}) - SUCCESS")
            
            # If an answer is found, print and return
            if response.answer:  # If answer section contains an IP, resolution is done
                print(f"[SUCCESS] {domain} -> {response.answer[0][0]}")
                return
            
            # Extract next set of nameservers if no direct answer is found
            next_ns_list = extract_next_nameservers(response)
##########
            stage = "TLD" if stage == "ROOT" else "AUTH"  # Move to the next stage
##########        
        else:
            print(f"[ERROR] Query failed for {ns_ip}")
            return  # Stop if query fails

    print("[ERROR] Resolution failed.")  # No valid response received

def recursive_dns_lookup(domain):
    """Performs recursive DNS resolution using the system's resolver."""
    print(f"[Recursive DNS Lookup] Resolving {domain}")

    try:
##########
        answer = dns.resolver.resolve(domain, "NS")  # Perform recursive query
##########
        for rdata in answer:
            print(f"[SUCCESS] {domain} -> {rdata}")
        
        answer = dns.resolver.resolve(domain, "A")  # Perform recursive query
        for rdata in answer:
            print(f"[SUCCESS] {domain} -> {rdata}")
    except Exception as e:
        print(f"[ERROR] Recursive lookup failed: {e}")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3 or sys.argv[1] not in {"iterative", "recursive"}:
        print("Usage: python3 dns_server.py <iterative|recursive> <domain>")
        sys.exit(1)

    mode = sys.argv[1] # Get mode (iterative or recursive)
    domain = sys.argv[2] # Get domain to resolve
    start_time = time.time() # Record start time

    # Execute the selected DNS resolution mode
    if mode == "iterative":
        iterative_dns_lookup(domain)
    else:
        recursive_dns_lookup(domain)
    
    print(f"Time taken: {time.time() - start_time:.3f} seconds") # Print execution time
