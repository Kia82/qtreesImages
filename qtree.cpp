/**
 * @file qtree.cpp
 * @description student implementation of QTree class used for storing image data
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE
 */

#include "qtree.h"

/**
 * Constructor that builds a QTree out of the given PNG.
 * Every leaf in the tree corresponds to a pixel in the PNG.
 * Every non-leaf node corresponds to a rectangle of pixels
 * in the original PNG, represented by an (x,y) pair for the
 * upper left corner of the rectangle and an (x,y) pair for
 * lower right corner of the rectangle. In addition, the Node
 * stores a pixel representing the average color over the
 * rectangle.
 *
 * The average color for each node in your implementation MUST
 * be determined in constant time. HINT: this will lead to nodes
 * at shallower levels of the tree to accumulate some error in their
 * average color value, but we will accept this consequence in
 * exchange for faster tree construction.
 * Note that we will be looking for specific color values in our
 * autograder, so if you instead perform a slow but accurate
 * average color computation, you will likely fail the test cases!
 *
 * Every node's children correspond to a partition of the
 * node's rectangle into (up to) four smaller rectangles. The node's
 * rectangle is split evenly (or as close to evenly as possible)
 * along both horizontal and vertical axes. If an even split along
 * the vertical axis is not possible, the extra line will be included
 * in the left side; If an even split along the horizontal axis is not
 * possible, the extra line will be included in the upper side.
 * If a single-pixel-wide rectangle needs to be split, the NE and SE children
 * will be null; likewise if a single-pixel-tall rectangle needs to be split,
 * the SW and SE children will be null.
 *
 * In this way, each of the children's rectangles together will have coordinates
 * that when combined, completely cover the original rectangle's image
 * region and do not overlap.
 */
QTree::QTree(const PNG& imIn) {
	// ADD YOUR IMPLEMENTATION BELOW
	height = imIn.height();
	width = imIn.width();
	
	root->lowRight = pair<int, int>(width,height);

	
	root = BuildNode(imIn, make_pair(0,0), make_pair(width, height)); 


}
/**
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * @param img reference to the original input image.
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */
Node* QTree::BuildNode(const PNG& img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr) {
		RGBAPixel p1;

		Node* node = new Node(ul,lr, p1);	


		if(ul.first == lr.first && ul.second == lr.second){ //height and width = 1
		return new Node(ul, lr, *(img.getPixel(ul.first, ul.second)));
		
	} else if((lr.first - ul.first) == 1){ //difference in width	
		//height of img paritition 
		unsigned int img_size = lr.second - ul.second;
		
		// sets corners of NW 
		node->NW = new Node(ul, make_pair(ul.first, ul.second + (img_size)/2), p1);

		//sets corners of SW
		node->SW = new Node(make_pair(ul.first, (ul.second + (img_size)/2 +1)), lr , p1)
		

	} else if((lr.second - ul.second) == 1){ //difference in height
	
			//height of img paritition 
		unsigned int img_size = lr.first - ul.first;	
		// sets corners of NW and sets pixel to default constructor pixel
			
			node->NW = new Node(ul, make_pair(ul.first + (img_size)/2, ul.second), p1);
	
		//sets corner of NE
			node->NE = new Node(make_pair((ul.first + (img_size)/2 +1), ul.second), lr, p1);
	}
	//TODO 
	//set up regular parameters 
	//somehow find avg?


	node->NW = new Node(ul, make_pair(ul.first/2, ul.second/2), p1);
	node->NE = new Node(make_pair(width/2, height/2), , p1);

	node->SW = new Node(ul, make_pair(width/2, height/2), p1);
	node->SE = new Node(ul, make_pair(width/2, height/2), p1);
	// BuildNode(NW);
	// BuildNode(NE);
	// BuildNode(SW);
	// BuildNode(SE);


}

RGBAPixel QTree::getAverage(Node* node) {
    int red = 0;
    int green = 0;
    int blue = 0;
    

    if (node->NW != NULL) {
        red += node->NW->avg.r;
        green += node->NW->avg.g;
        blue += node->NW->avg.b;
    }

    if (node->NE != NULL) {
        red += node->NE->avg.r;
        green += node->NE->avg.g;
        blue += node->NE->avg.b;
    }

    if (node->SW != NULL) {
        red += node->SW->avg.r;
        green += node->SW->avg.g;
        blue += node->SW->avg.b;

    }

    if (node->SE != NULL) {
        red += node->SE->avg.r;
        green += node->SE->avg.g;
        blue += node->SE->avg.b;
    }

    RGBAPixel avg;
    avg.r = red / 3;
    avg.g = green / 3;
    avg.b = blue / 3;

    return avg;
}


/**
 * Overloaded assignment operator for QTrees.
 * Part of the Big Three that we must define because the class
 * allocates dynamic memory. This depends on your implementation
 * of the copy and clear funtions.
 *
 * @param rhs The right hand side of the assignment statement.
 */
QTree& QTree::operator=(const QTree& rhs) {
    // Check for self-assignment
    if (this != &rhs && isValid(rhs)) {

        root = Copy(rhs.root); 
    }

    return *this;
}


bool QTree::isValid(Node* node) {
    if (node == NULL) {
        return true;
    }

    int width = node->lowRight.first - node->upLeft.first;
    int height = node->lowRight.second - node->upLeft.second;

    if (width < 1 || height < 1) {
        return false;
    }

    if (width == 1 && height == 1) {
        return node->NW == NULL && node->NE == NULL && node->SW == NULL && node->SE == NULL;
    }

    if (width == 1) {
        return node->NE == NULL && node->SE == NULL && isValid(node->NW) && isValid(node->SW);
    }

    if (height == 1) {
        return node->SW == NULL && node->SE == NULL && isValid(node->NW) && isValid(node->NE);
    }

    return isValid(node->NW) && isValid(node->NE) && isValid(node->SW) && isValid(node->SE);
}


/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the
 * average color stored in the node.
 *
 * For up-scaled images, no color interpolation will be done;
 * each rectangle is fully rendered into a larger rectangular region.
 *
 * @param scale multiplier for each horizontal/vertical dimension
 * @pre scale > 0
 */
PNG QTree::Render(unsigned int scale) const {
    // Create a new PNG image
    PNG img(width * scale, height * scale);

    // Draw each leaf node's rectangle onto the PNG canvas
    drawRectangle(root, img, scale);

    return img;
}

void QTree::drawRectangle(Node* node, PNG& img, unsigned int scale) const {
    if (node == NULL) {
        return;
    }

    // If the node is a leaf node, draw its rectangle
    if (node->NW == NULL && node->NE == NULL && node->SW == NULL && node->SE == NULL) {

        for (int x = node->upLeft.first * scale; x < node->lowRight.first * scale; x++) {

            for (int y = node->upLeft.second * scale; y < node->lowRight.second * scale; y++) {
                RGBAPixel* pixel = img.getPixel(x, y);
                *pixel = node->avg;
            }
        }
    } else {
        // If the node is not a leaf node, recursively draw its children's rectangles
        drawRectangle(node->NW, img, scale);
        drawRectangle(node->NE, img, scale);
        drawRectangle(node->SW, img, scale);
        drawRectangle(node->SE, img, scale);
    }
}


/**
 *  Prune function trims subtrees as high as possible in the tree.
 *  A subtree is pruned (cleared) if all of the subtree's leaves are within
 *  tolerance of the average color stored in the root of the subtree.
 *  NOTE - you may use the distanceTo function found in RGBAPixel.h
 *  Pruning criteria should be evaluated on the original tree, not
 *  on any pruned subtree. (we only expect that trees would be pruned once.)
 *
 * You may want a recursive helper function for this one.
 *
 * @param tolerance maximum RGBA distance to qualify for pruning
 * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
 */
void QTree::Prune(double tolerance) {
	// ADD YOUR IMPLEMENTATION BELOW
	
}

/**
 *  FlipHorizontal rearranges the contents of the tree, so that
 *  its rendered image will appear mirrored across a vertical axis.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  After flipping, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel wide rectangles have
 *  null eastern children
 *  (i.e. after flipping, a node's NW and SW pointers may be null, but
 *  have non-null NE and SE)
 * 
 *  You may want a recursive helper function for this one.
 */
void QTree::FlipHorizontal() {
    flip(root);
}

void QTree::flip(Node* node) {
    if (node == NULL) {
        return;
    }

    // Swap the NW and SW children
    Node* temp = node->NW;
    node->NW = node->SW;
    node->SW = temp;

    // Swap the NE and SE children
    temp = node->NE;
    node->NE = node->SE;
    node->SE = temp;

    // Recursively flip the children
    flip(node->NW);
    flip(node->NE);
    flip(node->SW);
    flip(node->SE);
}


/**
 *  RotateCCW rearranges the contents of the tree, so that its
 *  rendered image will appear rotated by 90 degrees counter-clockwise.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  Note that this may alter the dimensions of the rendered image, relative
 *  to its original dimensions.
 *
 *  After rotation, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel tall or wide rectangles
 *  have null eastern or southern children
 *  (i.e. after rotation, a node's NW and NE pointers may be null, but have
 *  non-null SW and SE, or it may have null NW/SW but non-null NE/SE)
 *
 *  You may want a recursive helper function for this one.
 */
void QTree::RotateCCW() {
    rotate(root);
}

void QTree::rotate(Node* node) {
    if (node == NULL) {
        return;
    }

    // Swap the children in a counter-clockwise direction
    Node* temp = node->NW;
    node->NW = node->NE;
    node->NE = node->SE;
    node->SE = node->SW;
    node->SW = temp;

    // Recursively rotate the children
    rotate(node->NW);
    rotate(node->NE);
    rotate(node->SW);
    rotate(node->SE);
}

/void QTree::Clear() {
    clear(root);
    root = NULL;
}

void QTree::clear(Node* node) {
    if (node == NULL) {
        return;
    }

    // Recursively clear the children
    clear(node->NW);
    clear(node->NE);
    clear(node->SW);
    clear(node->SE);

    // Delete the node
    delete node;
}

void QTree::Copy(const QTree& other) {
    root = copy(other.root);
}

QTree::Node* QTree::copy(Node* node) {
    if (node == NULL) {
        return NULL;
    }

    // Create a new node and copy the data
    Node* newNode = new Node;
    newNode->upLeft = node->upLeft;
    newNode->lowRight = node->lowRight;
    newNode->avg = node->avg;

    // Recursively copy the children
    newNode->NW = copy(node->NW);
    newNode->NE = copy(node->NE);
    newNode->SW = copy(node->SW);
    newNode->SE = copy(node->SE);

    return newNode;
}
