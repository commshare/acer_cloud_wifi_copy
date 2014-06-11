#include <escore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static u8 cred_default[] = {
    0x01, 0x00,                 // version 1
    0x00, 0x00,                 // padding
    0x00, 0x00, 0x00, 0x00,     // pub type RSA4096
    0x00, 0x01, 0x00, 0x01,     // root_kpub_exp          
    // platform common key
    0xa2, 0xc8, 0x80, 0x41, 0xe2, 0x36, 0x73, 0x4b, 0x43, 0x6e, 0x76, 0x4b,
    0xcb, 0x83, 0x0f, 0x61,
    // rsa 4096 cert
    0xe5, 0x05, 0xce, 0xec, 0x5f, 0xf7, 0x61, 0xa7,
    0x8a, 0x38, 0xfc, 0x8c, 0x6e, 0x1e, 0x21, 0x53,
    0x39, 0x8c, 0x3c, 0x44, 0xd0, 0x04, 0xc8, 0x27,
    0xfa, 0xac, 0xbd, 0x06, 0x05, 0xe6, 0x88, 0x98,
    0x76, 0x32, 0xb3, 0x48, 0x7d, 0x96, 0x08, 0x74,
    0xab, 0xe4, 0xff, 0x3b, 0x2d, 0xbd, 0x20, 0x6d,
    0xed, 0xd0, 0x06, 0xd5, 0x03, 0x26, 0xd0, 0x1b,
    0x11, 0xba, 0x80, 0x4f, 0x15, 0x82, 0xb1, 0xd9,
    0x85, 0xc3, 0x97, 0xbd, 0x81, 0x30, 0x72, 0xd2,
    0x0e, 0x16, 0x18, 0x03, 0x0c, 0xc6, 0x75, 0xc3,
    0xf4, 0x7d, 0xb6, 0xf6, 0x91, 0x19, 0x49, 0xf8,
    0xab, 0x06, 0x2c, 0x5d, 0xf5, 0x19, 0xe4, 0x85,
    0x1f, 0x70, 0x94, 0xa6, 0x40, 0x0c, 0x88, 0x1a,
    0x3d, 0xdf, 0x8f, 0x61, 0x56, 0xc8, 0x88, 0xd1,
    0x05, 0x1e, 0xff, 0x5e, 0xda, 0x57, 0xff, 0x6e,
    0x80, 0x1f, 0x28, 0x6a, 0x89, 0x62, 0xad, 0x73,
    0x25, 0xd5, 0xdd, 0x8e, 0x6b, 0x5b, 0x5f, 0x84,
    0x4b, 0xf9, 0x58, 0xfa, 0xff, 0x60, 0xd7, 0x05,
    0xa4, 0x15, 0x22, 0x14, 0x0e, 0xea, 0xe6, 0x74,
    0x60, 0x30, 0x32, 0x75, 0x45, 0xeb, 0xfe, 0x99,
    0x58, 0xf8, 0x5c, 0x41, 0xce, 0xbb, 0xbd, 0xa7,
    0x8d, 0x22, 0x38, 0x61, 0x1b, 0x9d, 0x02, 0x3e,
    0x16, 0xf4, 0x86, 0x80, 0x87, 0xc9, 0xb2, 0xb2,
    0x67, 0xce, 0xbb, 0x71, 0xf5, 0x72, 0xc9, 0x56,
    0x95, 0x47, 0xd8, 0x43, 0xd8, 0x2a, 0x3a, 0x60,
    0x66, 0x7b, 0x3b, 0x2a, 0xd7, 0x65, 0x9c, 0xee,
    0x97, 0x67, 0x9b, 0xe3, 0x8f, 0x40, 0xd3, 0x5d,
    0xeb, 0xca, 0x81, 0x71, 0x9f, 0xbb, 0x93, 0x37,
    0x6b, 0x9f, 0x08, 0xc0, 0x31, 0x00, 0xe9, 0x86,
    0x1b, 0x34, 0x38, 0xc7, 0x65, 0xe2, 0xe6, 0xa5,
    0xe6, 0xea, 0x48, 0x04, 0x40, 0x3a, 0x21, 0xeb,
    0xce, 0xeb, 0x5d, 0x14, 0x73, 0x98, 0xda, 0x32,
    0x10, 0x62, 0x82, 0x1f, 0xab, 0x5b, 0x1a, 0x22,
    0xb8, 0x6e, 0x9f, 0x76, 0x74, 0x30, 0x33, 0x3a,
    0x51, 0x42, 0xe2, 0x9e, 0xf8, 0x26, 0x19, 0x18,
    0x05, 0xd6, 0xc2, 0x70, 0x45, 0x08, 0x8d, 0xdf,
    0xc9, 0x00, 0xa3, 0x48, 0xfb, 0xba, 0x10, 0xcd,
    0xbd, 0xfe, 0x57, 0xa2, 0x97, 0x89, 0x6b, 0x4a,
    0x62, 0xd2, 0xf5, 0xcc, 0x04, 0x95, 0x4f, 0xe8,
    0x01, 0x41, 0x29, 0xe6, 0x02, 0x0e, 0x98, 0x05,
    0x3f, 0x61, 0x8f, 0x19, 0xc2, 0x49, 0x8a, 0x3e,
    0x1e, 0xc6, 0xff, 0xc8, 0x23, 0xc3, 0xbe, 0x43,
    0x4a, 0x36, 0x2a, 0x32, 0xc5, 0x97, 0x9b, 0xfc,
    0x14, 0x30, 0xac, 0x04, 0xfb, 0xd6, 0x23, 0x86,
    0xd3, 0xae, 0xf2, 0xae, 0xef, 0x1f, 0x53, 0x1d,
    0x95, 0x9f, 0x78, 0xf8, 0x9f, 0xc8, 0xb8, 0x90,
    0x79, 0x00, 0x20, 0xb2, 0xa5, 0x73, 0xcb, 0x28,
    0x8d, 0xf6, 0xaa, 0xbd, 0x01, 0x74, 0x5b, 0x83,
    0x49, 0x63, 0xa4, 0x6a, 0x2f, 0xe3, 0x40, 0x82,
    0x7d, 0xa1, 0xd0, 0x62, 0xcd, 0xf1, 0x23, 0xb4,
    0x5b, 0xab, 0x35, 0x15, 0x18, 0xb7, 0x7a, 0x95,
    0xd3, 0x56, 0x67, 0xb9, 0xe9, 0x25, 0xe0, 0x11,
    0xd8, 0x20, 0x74, 0xeb, 0xc5, 0x28, 0xd9, 0x7e,
    0x6e, 0x58, 0x18, 0x52, 0xb8, 0xbe, 0x52, 0xbc,
    0xaf, 0xb5, 0xff, 0xfa, 0xfe, 0xca, 0xf1, 0x1c,
    0x67, 0x41, 0x43, 0xa8, 0x03, 0x59, 0x42, 0x8f,
    0x03, 0x85, 0x66, 0xb6, 0x6b, 0x91, 0x8d, 0x59,
    0xc1, 0xcc, 0x54, 0xa4, 0x3a, 0x9e, 0xb6, 0xbe,
    0x5e, 0xb6, 0x02, 0x90, 0x15, 0x5f, 0xe4, 0xd6,
    0x67, 0x40, 0x41, 0xa3, 0x18, 0x90, 0x1c, 0x34,
    0xcb, 0xbd, 0xb3, 0xb1, 0x1c, 0x28, 0xd4, 0xdd,
    0x94, 0x56, 0x6f, 0x16, 0x44, 0x51, 0x5d, 0x1d,
    0x33, 0x24, 0x67, 0xcf, 0x9d, 0x5e, 0xde, 0x27,
    0x3d, 0xcc, 0xc1, 0xb4, 0xd3, 0xa5, 0x02, 0xe1,
};

static u8 cred_secret[] = {
	0x00, 0x01, 0x00, 0x00, 0xa1, 0x60, 0x4a, 0x6a,
	0x71, 0x23, 0xb5, 0x29, 0xae, 0x8b, 0xec, 0x32,
	0xc8, 0x16, 0xfc, 0xaa, 0xe8, 0x71, 0xa3, 0x3e,
	0x0b, 0xbd, 0xbf, 0x23, 0x37, 0x29, 0x9f, 0xb5,
	0xab, 0x62, 0x56, 0x27, 0xa2, 0xc1, 0xa1, 0xf2,
	0x4d, 0x62, 0x84, 0xc5, 0x68, 0x1c, 0xd8, 0xb2,
	0xce, 0xcd, 0x82, 0x22, 0x00, 0x5a, 0x72, 0x2f,
	0xa9, 0x1e, 0x48, 0x74, 0x2d, 0x80, 0x1f, 0xf2,
	0x1d, 0xf7, 0x37, 0x08, 0x90, 0xdf, 0x03, 0x01,
	0x46, 0x1a, 0xcc, 0xc5, 0x36, 0x56, 0xf9, 0x29,
	0xc5, 0xab, 0x00, 0x00, 0x6e, 0xa8, 0xd2, 0x18,
	0xd6, 0x07, 0x22, 0xe3, 0xc4, 0x6f, 0xd8, 0x95,
	0x84, 0xde, 0x52, 0xe4, 0x28, 0x08, 0x91, 0x98,
	0x68, 0xe1, 0x97, 0x1e, 0x6e, 0x12, 0x2d, 0x6d,
	0x14, 0xab, 0x49, 0x24, 0x6d, 0xb8, 0x89, 0x57,
	0x53, 0x56, 0x7f, 0xc0, 0x23, 0x26, 0x91, 0x3c,
	0x89, 0xcd, 0x78, 0x42, 0x6e, 0x8c, 0x6b, 0x20,
	0xdd, 0x62, 0x13, 0x47, 0xf2, 0xa9, 0x0d, 0x07,
	0x5b, 0xee, 0x54, 0xb8, 0x2e, 0x06, 0x37, 0x5b,
	0xc9, 0xc0, 0x70, 0xd6, 0xfa, 0x03, 0xe2, 0x94,
	0x86, 0xa0, 0xe5, 0x0d, 0xb0, 0x49, 0xd0, 0x5f,
	0x65, 0xd3, 0x3e, 0x16, 0xa0, 0xb9, 0x2b, 0x7e,
	0xd3, 0xc6, 0xb2, 0xc8, 0x68, 0xff, 0xa2, 0x04,
	0xab, 0xa9, 0x22, 0x36, 0x9e, 0xf6, 0xe9, 0x7b,
	0xf6, 0x86, 0x09, 0x8c, 0x01, 0xa8, 0xf5, 0x7d,
	0xec, 0xad, 0x1d, 0x5b, 0x16, 0x31, 0x71, 0x1d,
	0x54, 0xd4, 0x10, 0x46,
};

static u8 cred_clear[] = {
	0x00, 0x01, 0x00, 0x00, 0xfb, 0x18, 0x2f, 0x39,
	0x4d, 0x36, 0x29, 0xd0, 0x00, 0x00, 0x00, 0x02,
	0x01, 0xec, 0xb0, 0x21, 0xad, 0x6b, 0x53, 0x66,
	0x41, 0x7e, 0xd9, 0x59, 0x72, 0xcc, 0x7c, 0xe1,
	0xde, 0xda, 0xdb, 0x3f, 0x0c, 0xc2, 0xb7, 0xe2,
	0xf4, 0xd4, 0xf4, 0xc3, 0x62, 0x48, 0x01, 0x24,
	0x8c, 0x0a, 0x13, 0x46, 0x8c, 0x66, 0x5c, 0x8e,
	0x4f, 0x2b, 0x74, 0x7b, 0x5a, 0xf2, 0xb9, 0xd9,
	0x81, 0x72, 0x9b, 0x3b, 0xb6, 0xc6, 0xa7, 0x60,
	0xf1, 0xd6, 0x76, 0x0f, 0x00, 0x01, 0x00, 0x05,
	0x00, 0xe5, 0x5c, 0x71, 0xc9, 0xbe, 0x9b, 0xd6,
	0x3b, 0x8f, 0x46, 0x3e, 0xfa, 0x18, 0x9d, 0x25,
	0xea, 0x5e, 0xa3, 0x7c, 0x5c, 0x5c, 0xb0, 0x3e,
	0x24, 0x02, 0x73, 0xef, 0xd1, 0x02, 0x00, 0xfd,
	0xb8, 0xf8, 0x0f, 0x45, 0xcf, 0xc8, 0x07, 0x13,
	0x0f, 0xaf, 0x6c, 0x10, 0xd1, 0xfa, 0x87, 0x78,
	0x44, 0xee, 0x97, 0xfb, 0xbc, 0x5c, 0xff, 0x51,
	0x60, 0x2b, 0xab, 0x09, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6f, 0x74,
	0x2d, 0x43, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x32, 0x2d, 0x4d, 0x53, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x56, 0x50, 0x66, 0x62, 0x31, 0x38, 0x32, 0x66,
	0x33, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x72, 0xce, 0x2f, 0xd0, 0x01, 0xec, 0xb0, 0x21,
	0xad, 0x6b, 0x53, 0x66, 0x41, 0x7e, 0xd9, 0x59,
	0x72, 0xcc, 0x7c, 0xe1, 0xde, 0xda, 0xdb, 0x3f,
	0x0c, 0xc2, 0xb7, 0xe2, 0xf4, 0xd4, 0xf4, 0xc3,
	0x62, 0x48, 0x01, 0x24, 0x8c, 0x0a, 0x13, 0x46,
	0x8c, 0x66, 0x5c, 0x8e, 0x4f, 0x2b, 0x74, 0x7b,
	0x5a, 0xf2, 0xb9, 0xd9, 0x81, 0x72, 0x9b, 0x3b,
	0xb6, 0xc6, 0xa7, 0x60, 0xf1, 0xd6, 0x76, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6f, 0x74,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xd0, 0x1f, 0xe1, 0x00, 0xd4, 0x35, 0x56, 0xb2,
	0x4b, 0x56, 0xda, 0xe9, 0x71, 0xb5, 0xa5, 0xd3,
	0x84, 0xb9, 0x30, 0x03, 0xbe, 0x1b, 0xbf, 0x28,
	0xa2, 0x30, 0x5b, 0x06, 0x06, 0x45, 0x46, 0x7d,
	0x5b, 0x02, 0x51, 0xd2, 0x56, 0x1a, 0x27, 0x4f,
	0x9e, 0x9f, 0x9c, 0xec, 0x64, 0x61, 0x50, 0xab,
	0x3d, 0x2a, 0xe3, 0x36, 0x68, 0x66, 0xac, 0xa4,
	0xba, 0xe8, 0x1a, 0xe3, 0xd7, 0x9a, 0xa6, 0xb0,
	0x4a, 0x8b, 0xcb, 0xa7, 0xe6, 0xfb, 0x64, 0x89,
	0x45, 0xeb, 0xdf, 0xdb, 0x85, 0xba, 0x09, 0x1f,
	0xd7, 0xd1, 0x14, 0xb5, 0xa3, 0xa7, 0x80, 0xe3,
	0xa2, 0x2e, 0x6e, 0xcd, 0x87, 0xb5, 0xa4, 0xc6,
	0xf9, 0x10, 0xe4, 0x03, 0x22, 0x08, 0x81, 0x4b,
	0x0c, 0xee, 0xa1, 0xa1, 0x7d, 0xf7, 0x39, 0x69,
	0x5f, 0x61, 0x7e, 0xf6, 0x35, 0x28, 0xdb, 0x94,
	0x96, 0x37, 0xa0, 0x56, 0x03, 0x7f, 0x7b, 0x32,
	0x41, 0x38, 0x95, 0xc0, 0xa8, 0xf1, 0x98, 0x2e,
	0x15, 0x65, 0xe3, 0x8e, 0xed, 0xc2, 0x2e, 0x59,
	0x0e, 0xe2, 0x67, 0x7b, 0x86, 0x09, 0xf4, 0x8c,
	0x2e, 0x30, 0x3f, 0xbc, 0x40, 0x5c, 0xac, 0x18,
	0x04, 0x2f, 0x82, 0x20, 0x84, 0xe4, 0x93, 0x68,
	0x03, 0xda, 0x7f, 0x41, 0x34, 0x92, 0x48, 0x56,
	0x2b, 0x8e, 0xe1, 0x2f, 0x78, 0xf8, 0x03, 0x24,
	0x63, 0x30, 0xbc, 0x7b, 0xe7, 0xee, 0x72, 0x4a,
	0xf4, 0x58, 0xa4, 0x72, 0xe7, 0xab, 0x46, 0xa1,
	0xa7, 0xc1, 0x0c, 0x2f, 0x18, 0xfa, 0x07, 0xc3,
	0xdd, 0xd8, 0x98, 0x06, 0xa1, 0x1c, 0x9c, 0xc1,
	0x30, 0xb2, 0x47, 0xa3, 0x3c, 0x8d, 0x47, 0xde,
	0x67, 0xf2, 0x9e, 0x55, 0x77, 0xb1, 0x1c, 0x43,
	0x49, 0x3d, 0x5b, 0xba, 0x76, 0x34, 0xa7, 0xe4,
	0xe7, 0x15, 0x31, 0xb7, 0xdf, 0x59, 0x81, 0xfe,
	0x24, 0xa1, 0x14, 0x55, 0x4c, 0xbd, 0x8f, 0x00,
	0x5c, 0xe1, 0xdb, 0x35, 0x08, 0x5c, 0xcf, 0xc7,
	0x78, 0x06, 0xb6, 0xde, 0x25, 0x40, 0x68, 0xa2,
	0x6c, 0xb5, 0x49, 0x2d, 0x45, 0x80, 0x43, 0x8f,
	0xe1, 0xe5, 0xa9, 0xed, 0x75, 0xc5, 0xed, 0x45,
	0x1d, 0xce, 0x78, 0x94, 0x39, 0xcc, 0xc3, 0xba,
	0x28, 0xa2, 0x31, 0x2a, 0x1b, 0x87, 0x19, 0xef,
	0x0f, 0x73, 0xb7, 0x13, 0x95, 0x0c, 0x02, 0x59,
	0x1a, 0x74, 0x62, 0xa6, 0x07, 0xf3, 0x7c, 0x0a,
	0xa7, 0xa1, 0x8f, 0xa9, 0x43, 0xa3, 0x6d, 0x75,
	0x2a, 0x5f, 0x41, 0x92, 0xf0, 0x13, 0x61, 0x00,
	0xaa, 0x9c, 0xb4, 0x1b, 0xbe, 0x14, 0xbe, 0xb1,
	0xf9, 0xfc, 0x69, 0x2f, 0xdf, 0xa0, 0x94, 0x46,
	0xde, 0x5a, 0x9d, 0xde, 0x2c, 0xa5, 0xf6, 0x8c,
	0x1c, 0x0c, 0x21, 0x42, 0x92, 0x87, 0xcb, 0x2d,
	0xaa, 0xa3, 0xd2, 0x63, 0x75, 0x2f, 0x73, 0xe0,
	0x9f, 0xaf, 0x44, 0x79, 0xd2, 0x81, 0x74, 0x29,
	0xf6, 0x98, 0x00, 0xaf, 0xde, 0x6b, 0x59, 0x2d,
	0xc1, 0x98, 0x82, 0xbd, 0xf5, 0x81, 0xcc, 0xab,
	0xf2, 0xcb, 0x91, 0x02, 0x9e, 0xf3, 0x5c, 0x4c,
	0xfd, 0xbb, 0xff, 0x49, 0xc1, 0xfa, 0x1b, 0x2f,
	0xe3, 0x1d, 0xe7, 0xa5, 0x60, 0xec, 0xb4, 0x7e,
	0xbc, 0xfe, 0x32, 0x42, 0x5b, 0x95, 0x6f, 0x81,
	0xb6, 0x99, 0x17, 0x48, 0x7e, 0x3b, 0x78, 0x91,
	0x51, 0xdb, 0x2e, 0x78, 0xb1, 0xfd, 0x2e, 0xbe,
	0x7e, 0x62, 0x6b, 0x3e, 0xa1, 0x65, 0xb4, 0xfb,
	0x00, 0xcc, 0xb7, 0x51, 0xaf, 0x50, 0x73, 0x29,
	0xc4, 0xa3, 0x93, 0x9e, 0xa6, 0xdd, 0x9c, 0x50,
	0xa0, 0xe7, 0x38, 0x6b, 0x01, 0x45, 0x79, 0x6b,
	0x41, 0xaf, 0x61, 0xf7, 0x85, 0x55, 0x94, 0x4f,
	0x3b, 0xc2, 0x2d, 0xc3, 0xbd, 0x0d, 0x00, 0xf8,
	0x79, 0x8a, 0x42, 0xb1, 0xaa, 0xa0, 0x83, 0x20,
	0x65, 0x9a, 0xc7, 0x39, 0x5a, 0xb4, 0xf3, 0x29,
	0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
	0x90, 0x56, 0xb5, 0x0c, 0x36, 0x30, 0xeb, 0xf9,
	0xc8, 0x80, 0xd7, 0xb7, 0x74, 0x78, 0x04, 0x14,
	0xb8, 0xa9, 0x35, 0x89, 0x5f, 0xd4, 0x87, 0xbb,
	0xa0, 0x13, 0x52, 0xc2, 0x6f, 0x62, 0x99, 0x18,
	0x3e, 0x68, 0xeb, 0xf0, 0xff, 0x79, 0xd4, 0x2e,
	0xf6, 0xb1, 0x8c, 0xb6, 0x30, 0x5d, 0xb3, 0x9b,
	0x18, 0xa9, 0x12, 0x33, 0x1a, 0x5f, 0xa6, 0xab,
	0x3f, 0x50, 0xcb, 0xb8, 0xdd, 0xab, 0x4f, 0x66,
	0xc5, 0xa3, 0x58, 0x35, 0x8f, 0x4a, 0x48, 0x33,
	0x4e, 0x3a, 0x86, 0x68, 0x6e, 0x66, 0x8c, 0x0c,
	0xbe, 0x84, 0x47, 0xa1, 0x66, 0xcb, 0xcb, 0x4e,
	0xfd, 0xf6, 0x25, 0x37, 0x30, 0x07, 0x6c, 0x2a,
	0x78, 0x51, 0x56, 0x94, 0xcd, 0x4c, 0xf4, 0x91,
	0x98, 0x9f, 0xa5, 0x09, 0x04, 0x41, 0x72, 0x50,
	0x7a, 0x51, 0xea, 0x8e, 0x71, 0xc6, 0x1a, 0x94,
	0x33, 0xb0, 0x2a, 0x07, 0xa5, 0xf1, 0x26, 0x57,
	0x42, 0x4d, 0x44, 0x75, 0x2a, 0xfe, 0x65, 0x56,
	0x96, 0x36, 0x33, 0xc7, 0xc3, 0x97, 0x98, 0x2a,
	0x71, 0xa4, 0x80, 0x7f, 0x09, 0x78, 0x9a, 0xee,
	0xfb, 0x2b, 0x07, 0x69, 0xcb, 0x1d, 0xa4, 0xe0,
	0xca, 0xa7, 0x0b, 0xb8, 0xf6, 0xfd, 0xcf, 0x46,
	0xbd, 0x35, 0xc0, 0x8a, 0x00, 0x9c, 0x75, 0xab,
	0x9f, 0x52, 0x23, 0xc9, 0x5f, 0xf7, 0xda, 0xae,
	0x2f, 0x53, 0xe8, 0xe1, 0xaf, 0x25, 0x0d, 0xab,
	0x1d, 0x1b, 0xb2, 0xd7, 0x6a, 0x4a, 0x33, 0x38,
	0x69, 0x95, 0x38, 0xc6, 0xcd, 0xfe, 0xbc, 0x72,
	0xd0, 0xc3, 0x69, 0xec, 0x1f, 0x91, 0x9e, 0x81,
	0xee, 0x2b, 0x40, 0x90, 0x6c, 0xfd, 0xd3, 0x69,
	0x97, 0xab, 0x49, 0xd3, 0xa9, 0xd5, 0x91, 0xdd,
	0xc8, 0x42, 0xa0, 0xe4, 0x89, 0x7b, 0xc8, 0x8b,
	0xd0, 0x21, 0x67, 0x4f, 0x7e, 0xbc, 0xb2, 0x00,
	0xa3, 0xc4, 0x21, 0xb3, 0x95, 0x7f, 0x65, 0x50,
	0x9c, 0xd1, 0xc3, 0x2c, 0xb5, 0x5e, 0xeb, 0xb4,
	0x62, 0x99, 0xb8, 0x27, 0xd6, 0x2e, 0x53, 0x63,
	0x2c, 0xff, 0x70, 0x07, 0xbf, 0xfb, 0x5d, 0xcf,
	0x84, 0x15, 0xfd, 0x3f, 0xc3, 0xd8, 0x34, 0xe3,
	0x6b, 0xf3, 0x1f, 0x59, 0x90, 0x9f, 0x2f, 0x66,
	0xc6, 0xea, 0xc1, 0xfd, 0xfb, 0x34, 0x20, 0xa8,
	0xec, 0x94, 0xdd, 0x25, 0xf1, 0x04, 0x10, 0xf3,
	0x08, 0x33, 0xd8, 0xcb, 0x08, 0x70, 0x2e, 0x4a,
	0x8d, 0x27, 0xcc, 0xa7, 0x5b, 0xcb, 0x5a, 0xc3,
	0xeb, 0x30, 0x96, 0xec, 0x64, 0xf8, 0xf5, 0x28,
	0xdd, 0xfe, 0x51, 0xf6, 0x26, 0x9c, 0xf6, 0x9e,
	0x8d, 0x79, 0x4f, 0x1a, 0xd0, 0x46, 0x04, 0x06,
	0xfe, 0x79, 0xec, 0x98, 0x00, 0x9c, 0x6a, 0x3e,
	0xe7, 0xbd, 0x3d, 0x26, 0x30, 0x44, 0x06, 0x6b,
	0xa1, 0xf4, 0x58, 0xb6, 0x13, 0x40, 0x13, 0x8b,
	0x03, 0x32, 0xa3, 0x10, 0xb8, 0xd3, 0xc2, 0x3e,
	0xac, 0x22, 0xb0, 0xd6, 0x06, 0xa9, 0x8f, 0x51,
	0x98, 0xd8, 0xcf, 0x3a, 0xc7, 0x63, 0x85, 0x04,
	0xff, 0x0a, 0x02, 0xd1, 0x37, 0xaf, 0x4a, 0x7b,
	0xb8, 0x91, 0x46, 0x95, 0x25, 0xf3, 0x6c, 0xeb,
	0xfd, 0xcb, 0x0c, 0xd7, 0xf2, 0xd5, 0xe7, 0x2e,
	0xce, 0xdc, 0x26, 0xfe, 0x5d, 0xb2, 0x0b, 0x71,
	0xab, 0x5b, 0x47, 0xd3, 0xa4, 0xc9, 0xef, 0xeb,
	0xf4, 0x2f, 0x48, 0x48, 0xab, 0xf7, 0xf5, 0x63,
	0x79, 0x97, 0x08, 0x8a, 0x4c, 0x75, 0x0d, 0xde,
	0x29, 0x0f, 0x18, 0x77, 0x8b, 0x2b, 0x8c, 0x3d,
	0x60, 0x17, 0xeb, 0x2a, 0xda, 0x37, 0x4e, 0x1d,
	0x17, 0xef, 0xf9, 0x98, 0xe8, 0xbf, 0xb2, 0x53,
	0xde, 0xe2, 0xf4, 0xb5, 0x63, 0x02, 0x62, 0x95,
	0xeb, 0x89, 0x49, 0xb5, 0xb4, 0xd1, 0x03, 0xa2,
	0xf5, 0x00, 0x11, 0x5f, 0x45, 0xa1, 0xd0, 0xf2,
	0x3b, 0x92, 0xc0, 0x34, 0x06, 0xe4, 0xb6, 0xd1,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6f, 0x74,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x43, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x65, 0x64, 0x8f, 0x2b, 0xc9, 0xcc, 0x2d, 0xc4,
	0xdf, 0x29, 0x30, 0xe4, 0xdf, 0x3f, 0x8c, 0x70,
	0xa0, 0x78, 0x94, 0x87, 0x75, 0xad, 0x5e, 0x9a,
	0xa6, 0x04, 0xc5, 0xb4, 0xd8, 0xea, 0xff, 0x2a,
	0xa1, 0xd2, 0x14, 0x67, 0x65, 0x64, 0xef, 0xca,
	0x28, 0xcc, 0x00, 0x15, 0x45, 0x54, 0xa1, 0xa3,
	0xea, 0x13, 0x79, 0xe9, 0xe6, 0xca, 0xac, 0xed,
	0x15, 0x93, 0xfe, 0x88, 0xd8, 0x9a, 0xc6, 0xb8,
	0xac, 0xcc, 0xab, 0x6e, 0x20, 0x7c, 0xeb, 0x7c,
	0xca, 0x29, 0x80, 0x9e, 0x29, 0x80, 0x44, 0x06,
	0x62, 0xb7, 0xd4, 0x38, 0x2a, 0x15, 0xda, 0x43,
	0x08, 0x57, 0x45, 0xa9, 0xaa, 0xe5, 0x9a, 0xa0,
	0x5b, 0xdb, 0x32, 0xf6, 0x68, 0x69, 0xa2, 0xdd,
	0x42, 0x95, 0x38, 0x6c, 0x87, 0xec, 0xdd, 0x35,
	0x08, 0xa2, 0xcf, 0x60, 0xd0, 0x1e, 0x23, 0xec,
	0x2f, 0xe6, 0x98, 0xf4, 0x70, 0xd6, 0x00, 0x15,
	0x49, 0xa2, 0xf0, 0x67, 0x59, 0x13, 0x1e, 0x53,
	0x4c, 0x70, 0x06, 0x05, 0x7d, 0xef, 0x1d, 0x18,
	0xa8, 0x3f, 0x0a, 0xc7, 0x9c, 0xfe, 0x80, 0xff,
	0x5a, 0x91, 0xf2, 0xbe, 0xd4, 0xa0, 0x83, 0x70,
	0x61, 0x19, 0x0a, 0x03, 0x29, 0x90, 0x21, 0x65,
	0x40, 0x3c, 0x9a, 0x90, 0x8f, 0xb6, 0x15, 0x73,
	0x9f, 0x3c, 0xe3, 0x3b, 0xf1, 0xba, 0xea, 0x16,
	0xc2, 0x5b, 0xce, 0xd7, 0x96, 0x3f, 0xac, 0xc9,
	0xd2, 0x4d, 0x9c, 0x0a, 0xd7, 0x6f, 0xc0, 0x20,
	0xb2, 0xc4, 0xb8, 0x4c, 0x10, 0xa7, 0x41, 0xa2,
	0xcc, 0x7d, 0x9b, 0xac, 0x3a, 0xac, 0xcc, 0xa3,
	0x52, 0x9b, 0xac, 0x31, 0x6a, 0x9a, 0xa7, 0x5d,
	0x2a, 0x26, 0xc7, 0xd7, 0xd2, 0x88, 0xcb, 0xa4,
	0x66, 0xc5, 0xfe, 0x5f, 0x45, 0x4a, 0xe6, 0x79,
	0x74, 0x4a, 0x90, 0xa1, 0x57, 0x72, 0xdb, 0x3b,
	0x0e, 0x47, 0xa4, 0x9a, 0xf0, 0x31, 0xd1, 0x6d,
	0xbe, 0xab, 0x33, 0x2b, 0x00, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
	0xa6, 0x5a, 0x2b, 0x92, 0xc0, 0xe7, 0xaf, 0x0c,
	0x11, 0xd2, 0x5a, 0x9a, 0x6d, 0xc2, 0x9b, 0xc3,
	0xdd, 0xbf, 0x0f, 0x30, 0x17, 0xb1, 0xbe, 0x1f,
	0xd5, 0x29, 0xe9, 0x99, 0x73, 0x06, 0x67, 0x52,
	0x04, 0x05, 0xcf, 0x34, 0x0c, 0x28, 0x6d, 0xbb,
	0xa7, 0x69, 0x2e, 0x7e, 0xd2, 0x1f, 0x44, 0x43,
	0xca, 0x15, 0x22, 0x68, 0xb4, 0x6f, 0x7d, 0x03,
	0x63, 0xfb, 0xaa, 0x89, 0x3d, 0x0b, 0x6d, 0xc0,
	0xbc, 0x2d, 0x89, 0x3c, 0xb5, 0xe6, 0xf8, 0x97,
	0x97, 0x94, 0x09, 0xbc, 0x4d, 0x1b, 0x56, 0x18,
	0xd4, 0x50, 0x9d, 0xc2, 0xce, 0x1b, 0xc3, 0x97,
	0x22, 0xc1, 0x38, 0xa2, 0x29, 0x2c, 0x11, 0x42,
	0xe0, 0xbb, 0x14, 0xaa, 0x7a, 0xd8, 0x6b, 0xb3,
	0x28, 0x76, 0xa6, 0xf1, 0xbd, 0x7d, 0x80, 0x99,
	0xd0, 0xea, 0x7a, 0xf5, 0xc4, 0x47, 0x52, 0x01,
	0x28, 0x6b, 0x27, 0x08, 0xae, 0xfa, 0x68, 0x97,
	0x50, 0x2d, 0x91, 0x4d, 0x89, 0x96, 0xc9, 0xde,
	0xac, 0x4a, 0x4f, 0x31, 0x7f, 0xb8, 0x7c, 0xde,
	0xfd, 0x59, 0xe8, 0xc7, 0xa0, 0x65, 0xb7, 0x73,
	0xaa, 0xb0, 0xea, 0x80, 0xe6, 0x58, 0x2b, 0xc0,
	0xf7, 0x89, 0xa8, 0x84, 0x3d, 0xb6, 0x59, 0xac,
	0xb5, 0xb2, 0x43, 0x0e, 0x20, 0xa7, 0xaa, 0xc3,
	0x93, 0xa0, 0x8f, 0x66, 0x37, 0xe8, 0xf3, 0x83,
	0x14, 0x28, 0x71, 0x1a, 0x0b, 0x47, 0x8d, 0x21,
	0xe6, 0x5a, 0xc1, 0xf3, 0x26, 0xd2, 0x41, 0x75,
	0x90, 0xda, 0xb1, 0x05, 0x64, 0xbc, 0xd4, 0xfe,
	0xd0, 0xcb, 0x9b, 0x92, 0xd8, 0x27, 0xdc, 0x93,
	0x05, 0xc1, 0x21, 0xac, 0x59, 0xb1, 0x08, 0xdc,
	0xff, 0xec, 0xd5, 0x53, 0x56, 0xb4, 0xc3, 0x90,
	0x32, 0xa0, 0xc4, 0xfe, 0xe4, 0x8a, 0x57, 0xa5,
	0xad, 0x32, 0x9d, 0x94, 0x1e, 0x16, 0x29, 0xea,
	0x57, 0x10, 0x87, 0x82, 0x54, 0xc7, 0xec, 0xc1,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x52, 0x6f, 0x6f, 0x74,
	0x2d, 0x43, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x4d, 0x53, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x33, 0x36, 0xc6, 0xac, 0x01, 0xee, 0xff, 0xd1,
	0x3d, 0x2b, 0x54, 0xc4, 0x4d, 0xe5, 0xe9, 0x28,
	0x5e, 0x16, 0x98, 0x20, 0xdc, 0x0e, 0x61, 0x54,
	0x62, 0xc7, 0x23, 0x90, 0x0b, 0xa7, 0xba, 0xc3,
	0x2b, 0x5a, 0x00, 0xfe, 0x7a, 0xb1, 0x38, 0x5c,
	0x83, 0x15, 0xf5, 0x27, 0x40, 0x2a, 0x8b, 0xfc,
	0x50, 0x11, 0x82, 0x80, 0x89, 0x6c, 0xf8, 0x24,
	0x9e, 0x9d, 0x0d, 0x42, 0xf0, 0xda, 0xed, 0x31,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

int es_core_sanity_test()
{
    int err = 0;
    int rv;
    
    // Initialize the ES library
    rv = ESCore_Init(cred_default, sizeof(cred_default));
    if ( rv < 0 ) {
        printf("ESCore_Init failed with error code %d\n", rv);
        err = -1;
        goto done;
    }

    if ( ESCore_LoadCredentials(cred_secret, sizeof(cred_secret),
                     cred_clear, sizeof(cred_clear), ESCORE_INIT_MODE_INSECURE_CRED) ) {
        printf("Failed initializing ESC driver\n");
        err = -1;
        goto done;
    }

    u64 guid;
    rv = ESCore_GetDeviceGuid(&guid);
    if (rv < 0) {
        printf("ESCore_GetDeviceGuid failed: %d\n", rv);
        err = -1;
        goto done;
    }
    
done:
    printf("TC_RESULT=%s ;;; TC_NAME=es_decrypt_test\n", err ? "FAIL" : "PASS");
}
