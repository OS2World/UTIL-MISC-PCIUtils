#ifndef OEMHLP_PCI
#define OEMHLP_PCI			0x0b
#endif
#define OEMHLP_PCI_Subfunction_Read	0x03
#define OEMHLP_PCI_Subfunction_Write	0x04

#define IOCtlError_Invalid_Parameter	0xff13

#pragma pack(1)

struct Parameter_Packet_Read_PCI_Configuration_Space {
  UCHAR Subfunction_Number;
  UCHAR Bus_Number;
  UCHAR DevFuncNum;
  UCHAR Configuration_Register;
  UCHAR Size;
};  

struct Data_Packet_Read_PCI_Configuration_Space {
  UCHAR Return_Code;
  ULONG Data;
};  

struct Parameter_Packet_Write_PCI_Configuration_Space {
  UCHAR Subfunction_Number;
  UCHAR Bus_Number;
  UCHAR DevFuncNum;
  UCHAR Configuration_Register;
  UCHAR Size;
  ULONG Data;  
};  

struct Data_Packet_Write_PCI_Configuration_Space {
  UCHAR Return_Code;
};  
