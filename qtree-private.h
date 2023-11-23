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

RGBAPixel calculateAverageColor(const PNG & img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr);

void renderNode(PNG & img, Node* node, unsigned int scale) const;

void flipHorizontalHelper(Node* node);

void clearHelper(Node* node);

Node* copyHelper(Node* otherNode);

void rotateCCWHelper(Node* node);

void pruneHelper(Node* node, double tolerance);

bool allLeavesWithinTolerance(Node* node, const RGBAPixel& avg, double tolerance);

bool isLeaf(Node* node);