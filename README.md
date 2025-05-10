<div align="center">
   <h1>C++_FaceDetetion_RSID</h1>
</div>

### **Description**

This program facilitates **face authentication using faceprints**, including matching against a JSON local database and handling events such as detection and authentication results.



This manages a **simple in-memory database of face data** (faceprints), likely used with RSID. 



This handles **loading faceprint data from a JSON file, accessing entries, and adding new entries**.



### **Main Technology**

##### **RSIDWrapper class**

Interface with RSID face authentication pipeline.

Receive extracted faceprints and compare them to a local face database.

Support multi-threaded matching (e.g., for speed/scalability).

Interact with a JSON-based faceprint DB.

Handle facial detection and authentication events via callbacks.





##### RSIDWrapper class

Parses face data from a JSON file.

Stores and provides access to faceprint vectors.

Works with RSID face recognition (via `features[]`).



### **Contact Us**

For any inquiries or questions, please contact us.

telegram : @topdev1012

email :  skymorning523@gmail.com

Teams :  https://teams.live.com/l/invite/FEA2FDDFSy11sfuegI