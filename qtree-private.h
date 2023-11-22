/**
 * @file qtree-private.h
 * @description student declaration of private QTree functions
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE.
 * 
 *				Simply declare your function prototypes here.
 *              No other scaffolding is necessary.
 */

// begin your declarations below

RGBAPixel getAverage(Node* node);

bool isValid(Node* node);
void drawRectangle(Node* node, PNG& img, unsigned int scale) const;
void flip(Node* node);
void rotate(Node* node);