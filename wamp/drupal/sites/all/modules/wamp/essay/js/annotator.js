/**
 * @fileOverview This file defines the core classes used by WAMP Annotator UI, Corrector and Converter.
 * @author Pua Jun Hong
 */

// Constants
var CompareRanges = {
	START_TO_START: 0,
	START_TO_END: 1,
	END_TO_END: 2,
	END_TO_START: 3
};
var NodeType = {
	ELEMENT_NODE: 1,
	ATTRIBUTE_NODE: 2,
	TEXT_NODE: 3,
	CDATA_SECTION_NODE: 4,
	ENTITY_REFERENCE_NODE: 5,
	ENTITY_NODE: 6,
	PROCESSING_INSTRUCTION_NODE: 7,
	COMMENT_NODE: 8,
	DOCUMENT_NODE: 9,
	DOCUMENT_TYPE_NODE: 10,
	DOCUMENT_FRAGMENT_NODE: 11,
	NOTATION_NODE: 12
};
var KeyCode = {
	BACKSPACE: 8,
	CAPS_LOCK: 20,
	COMMA: 188,
	CONTROL: 17,
	DELETE: 46,
	DOWN: 40,
	END: 35,
	ENTER: 13,
	ESCAPE: 27,
	HOME: 36,
	INSERT: 45,
	LEFT: 37,
	NUMPAD_ADD: 107,
	NUMPAD_DECIMAL: 110,
	NUMPAD_DIVIDE: 111,
	NUMPAD_ENTER: 108,
	NUMPAD_MULTIPLY: 106,
	NUMPAD_SUBTRACT: 109,
	PAGE_DOWN: 34,
	PAGE_UP: 33,
	PERIOD: 190,
	RIGHT: 39,
	SHIFT: 16,
	SPACE: 32,
	TAB: 9,
	UP: 38
}

function debug() {
	if (typeof console != "undefined") {
		console.log.apply(this, arguments);
	}
}

/**
 * jQuery extensions
 */
(function($) {
	var draggables = [];
	
	function trackmove(event) {
		event.preventDefault();
		event.stopPropagation();
		
		for (var i=0; i<draggables.length; i++) {
			var elem = draggables[i];
			var offsetX = $(elem).data('drag-offsetX');
			var offsetY = $(elem).data('drag-offsetY');
			$(elem).css({
				top: event.pageY + offsetY + 'px',
				left: event.pageX + offsetX + 'px'
			});
		}
	}
	
	/**
	 * Start moving the selected elements with the mouse, with each maintaining the same 
	 * relative distance from the mouse when moving as when this function is called.
	 * @param {Event} event The jQuery.Event object to get the starting mouse position.
	 */
	jQuery.fn.startDrag = function(event) {
		event.preventDefault();
		
		if (draggables.length == 0) $(document).bind('mousemove', trackmove);
		for (var i=0; i<this.length; i++) {
			var elem = this.get(i);
			var elemOffset = $(elem).offset();
			if ($(elem).data('drag-offsetX') === undefined) {
				$(elem).data('drag-offsetX', elemOffset.left-event.pageX);
			}
			if ($(elem).data('drag-offsetY') === undefined) {
				$(elem).data('drag-offsetY', elemOffset.top-event.pageY);
			}
			draggables.push(this.get(i));
			//disableSelection(this.get(i));
		}
	}
	
	/**
	 * Stop moving the selected elements with the mouse 
	 * (that were moving with the mouse due to jQuery.fn.startDrag()).
	 */
	jQuery.fn.stopDrag = function() {
		var items = this;
		draggables = jQuery.grep(draggables, function(val, key) {
			if (items.index(val) == -1) {
				return true;
			} else {
				$(val).removeData('drag-offsetX');
				$(val).removeData('drag-offsetY');
				//enableSelection(val);
				return false;
			}
		});
		if (draggables.length == 0) $(document).unbind('mousemove', trackmove);
	}

	/**
	 * Check if the first selected element is an ancestor of descendent.
	 * @param {Node} descendent	The possible descendent node.
	 * @return {Boolean} true  If it is an ancestor of descendent.
	 *                   false Otherwise.
	 */
	jQuery.fn.ancestorOf = function(descendent) {
		var ancestor = this.get(0);
		if (ancestor && descendent && ancestor.childNodes && ancestor.childNodes.length > 0) {
			if (document.documentElement.compareDocumentPosition) {
				// if support DOM function compareDocumentPosition()
				return !!(ancestor.compareDocumentPosition(descendent) & 16);
			} else if (ancestor.contains && descendent.contains) {
				// if both ancestor and descendent are elements and we are in IE (having contains())
				return (ancestor.contains(descendent) && ancestor != descendent);
			} else {
				// all else fails, do it the primitive way...
				do {
					descendent = descendent.parentNode;
					if (descendent == ancestor) return true;
				} while (descendent);
			}
		}
		return false;
	}
	
	/**
	 * Check if the any of the selected elements has any of the given classes.
	 * @param {String[]} classes The list of HTML class names.
	 * @return {Boolean} true  If the any one has any of the given classes.
	 *                   false Otherwise.
	 */
	jQuery.fn.hasAnyClass = function(classes) {
		for (var n=0; n<this.length; n++) {
			for (var i=0; i<classes.length; i++) {
				if ($(this[n]).hasClass(classes[i])) return true;
			}
		}
		return false;
	}
})(jQuery);

/**
 * XMLTools static class.
 * @class Group of useful functions for parsing XML.
 */
XMLTools = {}
/**
 * Convert an DOM Node and it's subtree into an XML String.
 * @param {Node} xmlNode The DOM Node to convert.
 * @return {String} XML String representation of xmlNode (including it's contents, similar to outerHTML).
 */
XMLTools.xmlToString = function(xmlNode) {
	// Mozilla and Netscape browsers
	if (typeof XMLSerializer != "undefined") {
		return (new XMLSerializer()).serializeToString(xmlNode) ;
	}
	// MSIE
	else if (xmlNode.xml) {
		return xmlNode.xml;
	}
	else throw "serialize is not supported or can't serialize " + xmlNode;
}
/**
 * Convert an XML String into a DOM Document.
 * @param {String} xmlString The valid XML String representing a DOM Document or Node.
 * @return {Document} The DOM Document xmlString represents.
 */
XMLTools.stringToXML = function(xmlString) {
	var xml;
	
	// Mozilla and Netscape browsers
	if (document.implementation.createDocument) {
		var parser = new DOMParser();
		xml = parser.parseFromString(xmlString, "text/xml");
	}
	// MSIE
	else if (window.ActiveXObject) {
		xml = new ActiveXObject("Microsoft.XMLDOM");
		xml.async = 'false';
		xml.loadXML(xmlString);
	}

	return xml;
}
/**
 * Load an XML document synchronously from the url, blocking/hanging the browser until the loading is completed.
 * @param {String} url The URL address of the XML document file.
 * @return {Document} The DOM Document for the XML document.
 */
XMLTools.loadXMLDoc = function(url) {
	var xml;
	
	// code for IE
	if (window.ActiveXObject) {
		xml = new ActiveXObject('Microsoft.XMLDOM');
	}
	// code for Mozilla, Firefox, Opera, etc.
	else if (document.implementation && document.implementation.createDocument) {
		xml = document.implementation.createDocument('', '', null);
	}
	else {
		alert('Your browser cannot handle this script');
	}
	
	xml.async=false;
	xml.load(url);
	return(xml);
}

/**
 * Whitespace class constructor.
 * @class Tools to deal with (user-definable) whitespace in Strings.
 * @param {String} [whitespace='\n\r\t '] The string of whitespace characters.
 */
Whitespace = function(whitespace) {
	if (!whitespace) whitespace = '\n\r\t ';
	this.whitespace = whitespace;
}
/**
 * Left trim whitespace off str.
 * @param {String} str
 */
Whitespace.prototype.ltrim = function(str) {
	return str.replace(new RegExp('^['+this.whitespace+']+'), '');
}
/**
 * Right trim whitespace off str.
 * @param {String} str
 */
Whitespace.prototype.rtrim = function(str) {
	return str.replace(new RegExp('['+this.whitespace+']+$'), '');
}
/**
 * Trim (left and right) whitespace off str.
 * @param {String} str
 */
Whitespace.prototype.trim = function(str) {
	return this.ltrim(this.rtrim(str));
}
/**
 * Check if str consists of entirely whitespace characters.
 * @param {String} str
 */
Whitespace.prototype.isWhitespace = function(str) {
	return !(new RegExp('[^'+this.whitespace+']+').test(str));
}
/**
 * Move the offset forward, until the character in str at offset is not a whitespace character, or until the end of str is reached.
 * @param {String} str The string.
 * @param {Number} offset The integer offset.
 * @return {Number} The offset, snapped to the back of a non-whitespace character, or at the end of str.
 */
Whitespace.prototype.snapForward = function(str, offset) {
	while (offset < str.length-1 && this.isWhitespace(str.charAt(offset))) {
		offset++;
	}
	return offset;
}
/**
 * Move the offset backward, until the character in str at just before offset is not a whitespace character, or until the start of str is reached.
 * @param {String} str The string.
 * @param {Number} offset The integer offset.
 * @return {Number} The offset, snapped to the front of a non-whitespace character, or at the start of str.
 */
Whitespace.prototype.snapBackward = function(str, offset) {
	while (offset > 0 && this.isWhitespace(str.charAt(offset-1))) {
		offset--;
	}
	return offset;
}
/**
 * Condenses whitespace (replace multiple consequtive whitespaces with a single space) of the given string.
 * @param {String} str The string to condense.
 * @return {String} The condensed string.
 */
Whitespace.prototype.condense = function(str) {
	return str.replace(new RegExp('['+this.whitespace+']+', 'g'), ' ');
}
/**
 * Returns the condensed offset within an uncondensed string.<br />
 * (e.g. when str = '__a__b__c', actualOffset = 5, the condensed offset returned = 3)
 * @param {String} str The original uncondensed string.
 * @param {Number} actualOffset The actual offset within the uncondensed string.
 * @return {Number} The condensed offset.
 */
Whitespace.prototype.condenseOffset = function(str, actualOffset) {
	var condensedOffset = 0;
	for (var i=0; i<actualOffset && i<str.length; i++) {
		if (this.isWhitespace(str.charAt(i))) {
			while (i+1<str.length && this.isWhitespace(str.charAt(i+1))) {
				i++;
			}
		}
		condensedOffset++;
	}
	return condensedOffset;
}
/**
 * Returns the uncondensed/actual offset within an uncondensed string.<br />
 * (e.g. when str = '__a__b__c', condensedOffset = 3, the uncondensed offset returned = 5)
 * @param {String} str The original uncondensed string.
 * @param {Number} condensedOffset The condensed offset.
 * @return {Number} The actual uncondensed offset within the uncondensed string.
 */
Whitespace.prototype.uncondenseOffset = function(str, condensedOffset) {
	var actualOffset = 0;
	for (var i=0; i<condensedOffset && i<str.length; i++) {
		if (this.isWhitespace(str.charAt(actualOffset))) {
			while (actualOffset<str.length && this.isWhitespace(str.charAt(actualOffset))) {
				actualOffset++;
			}
		}
		else {
			actualOffset++;
		}
	}
	return actualOffset;
}

/**
 * Locator class constructor.
 * @class Locate a position within a specified DOM subtree, converting the location to/from an address string.
 * @param {Node} rootNode The root Node of the DOM subtree to find in.
 * @param {Object} [options] Set of optional settings.
 * @param {String[]} [options.normaliseClasses=[]] List of HTML class names of Elements to normalise.
 * @param {String[]} [options.ignoreClasses=[]] List of HTML class names of Elements to ignore.
 * @param {String} [options.whitespace=same as {@link Whitespace#constructor}'s default] String of whitespace characters.
 */
Locator = function(rootNode, options) {
	this.settings = jQuery.extend({
		normaliseClasses: [],
		ignoreClasses: [],
		whitespace: undefined
	}, options);
	this.rootNode = rootNode;
	this.whitespace = new Whitespace(this.settings.whitespace);
}
/**
 * Check if node can be ignored, but does NOT check if node is within an Element that can be ignored.
 * Ignorable nodes are whitespace-only Text nodes, 
 * Elements with any class found in settings.ignoreClasses,
 * and all other types of DOM nodes.
 * @param {Node} node The DOM node to check.
 * @return {Boolean} true  If node can be ignored,
 *                   false Otherwise.
 */
Locator.prototype.canIgnore = function(node) {
	if ((node.nodeType == NodeType.TEXT_NODE && this.whitespace.isWhitespace(node.data)) ||
		(node.nodeType == NodeType.ELEMENT_NODE && $(node).hasAnyClass(this.settings.ignoreClasses)) ||
		(node.nodeType != NodeType.TEXT_NODE && node.nodeType != NodeType.ELEMENT_NODE)) {
			return true;
	} else {
			return false;
	}
}
/**
 * Check if curNode, or both curNode and prevNode (if prevNode is a valid DOM Node), can be normalised, 
 * but does NOT check if curNode is within an Element that can be normalised.
 * Normalisable nodes are any Text nodes,
 * and Elements with any class found in settings.normaliseClasses.
 * @param {Node} curNode The DOM Node to check.
 * @param {Node} prevNode An optional additional DOM Node to check.
 * @return {Boolean} true  If curNode is normalisable, and prevNode is either null/undefined or is also normalisable.
 *                   false Otherwise.
 */
Locator.prototype.canNormalise = function(curNode, prevNode) {
	if (curNode.nodeType == NodeType.TEXT_NODE || $(curNode).hasAnyClass(this.settings.normaliseClasses)) {
		if (prevNode == null || (prevNode.nodeType == NodeType.TEXT_NODE || $(prevNode).hasAnyClass(this.settings.normaliseClasses))) {
			return true;
		} else {
			return false;
		}
	} else {
			return false;
	}
}
/**
 * Recursive function to return concatenated string of all text not within 
 * ignorable nodes descendent of the original given node (including the original node itself),
 * or the node's text itself, if the given node is a text node.
 * @param {Node} node The given node.
 * @return {String} The concatenated string.
 */
Locator.prototype.getNodeText = function(node) {
	if (node.nodeType == NodeType.TEXT_NODE) {
		return node.data;
	}
	else if (node.nodeType == NodeType.ELEMENT_NODE && !$(node).hasAnyClass(this.settings.ignoreClasses)) {
		var text = '';
		for (var i=0; i<node.childNodes.length; i++) {
			text += this.getNodeText(node.childNodes[i]);
		}
		return text;
	}
	else {
		return '';
	}
}
/**
 * Get the address of the given node-offset relative to rootNode.
 * @param {Node} node A DOM Node inside the DOM subtree under rootNode, which can be a Text or Element node.
 * @param {Number} offset The offset within node, which is the character-offset if node is a Text node, 
 *                        or a child-offset if node is an Element.
 * @return {String} The address string that represents the location of node-offset, relative to rootNode.
 */
Locator.prototype.getAddressFromNodeOffset = function(node, offset) {
	var addressStr = '';
	var str = '';
	
	//debug('getAddressFromNodeOffset', [node, offset]);
	
	// Cannot get address if node is not inside rootNode
	if ($(this.rootNode).ancestorOf(node) == false && this.rootNode != node) {
		return false;
	}
	
	// Move pointer out of ignorable elements or input elements ONLY (ignorable text nodes not included!)
	var tempNode = node, tempOffset = offset;
	if (tempNode.nodeType == NodeType.ELEMENT_NODE && tempOffset < tempNode.childNodes.length) {
		// move tempNode-tempOffset pointer inwards, if possible, to make sure node.childNodes[offset] itself is checked to see if it is ignorable
		tempNode = tempNode.childNodes[tempOffset];
		tempOffset = 0;
	}
	while (tempNode != this.rootNode) {
		if (tempNode.nodeType == NodeType.ELEMENT_NODE && (this.canIgnore(tempNode) || $(tempNode).is(':input'))) {
			offset = $(tempNode.parentNode.childNodes).index(tempNode);
			node = tempNode.parentNode;
		}
		tempOffset = $(tempNode.parentNode.childNodes).index(tempNode);
		tempNode = tempNode.parentNode;
	}
	
	// Make sure node eventually points to an element node
	if (node.nodeType == NodeType.TEXT_NODE) {
		// rtrim before checking (if not pure whitespace text), to remove extra whitespaces added to end of block elements by IE6/7
		if ((this.whitespace.isWhitespace(node.data) || offset < this.whitespace.rtrim(node.data).length)) { 
			// if offset is in middle of text node, get the substring from the start to offset, and move node pointer to start of text
			str = node.data.substring(0, offset);
			offset = $(node.parentNode.childNodes).index(node);
			node = node.parentNode;
		} else {
			// move pointer from end of text node to it's nextSibling
			offset = $(node.parentNode.childNodes).index(node) + 1;
			node = node.parentNode;
		}
	}
	
	// Move pointer to (child of) the nextSibling of the nearest parentNode while pointer is still at the end of a node,
	// to try and make addresses of a specific point unique
	while (node != this.rootNode && offset == node.childNodes.length) {
		offset = $(node.parentNode.childNodes).index(node) + 1;
		node = node.parentNode;
	}
	
	// Normalise adjacent normalisable nodes, recursively moving pointer to the start of this group of adjacent normalisable nodes while adding text to str
	//debug('a', [node, offset, node.childNodes[offset], str]);
	while (	(offset < node.childNodes.length && this.canNormalise(node.childNodes[offset])) ||
			((offset == node.childNodes.length || offset == 0) && this.canNormalise(node)) /* end or start of normalisable node */) {
		for (var i=offset-1; i>=0; i--) {
			var curNode = node.childNodes[i];
			if (this.canNormalise(curNode)) {
				// str = (non-ignorable text in curNode) + str
				str = this.getNodeText(curNode) + str;
				offset = i;
			}
			else if (curNode.nodeType != NodeType.TEXT_NODE && this.canIgnore(curNode)) {
				// note that you must check canIgnore() AFTER canNormalise(), since whitespace-only text nodes are considered ignorable,
				// but when calculating the text offset, such text nodes are actually significant and should be normalised
				offset = i;
			}
			else {
				// start of this group of adjacent normalisable nodes reached
				break;
			}
		}
		//debug('b', [node, offset, node.childNodes[offset], str]);
		
		// Move pointer up if start of normalisable node reached
		if (offset == 0 && this.canNormalise(node)) {
			offset = $(node.parentNode.childNodes).index(node);
			node = node.parentNode;
		}
		else {
			// start of this group of adjacent normalisable nodes reached
			break;
		}
		//debug('c', [node, offset, node.childNodes[offset], str]);
	}
	
	// Left trim whitespace before condensing to normalise behavior among different browsers,
	// e.g. Mozilla preserves (invisible) whitespace (e.g. spaces in <p>________hello</p>), but IE doesn't
	// Condense whitespace to normalise behavior among different browsers,
	// e.g. Mozilla preserves all whitespace as-is, while IE condenses whitespace
	var condensedOffset = this.whitespace.condense(this.whitespace.ltrim(str)).length;
	if (condensedOffset > 0) {
		addressStr = '.'+condensedOffset + addressStr;
	}
	//debug([str, this.whitespace.condense(this.whitespace.ltrim(str)).length, condensedOffset]);
	
	// Calculate child sequence (normalisedOffsets) from node up to rootNode,
	// ignoring (not incrementing offset for) every ignorable (whitespace-only or non-element) node,
	// and normalising groups of (from the 2nd onwards) adjacent text nodes and/or elements with a normalisable class 
	do {
		var normalisedOffset = 0;
		var prevNode = null;
		for (var i=0; i<offset; i++) {
			var curNode = node.childNodes[i];
			if (this.canIgnore(curNode)) {
				continue;
			}
			else if (prevNode && this.canNormalise(curNode, prevNode)) {
				prevNode = curNode;
				continue;
			}
			else {
				prevNode = curNode;
				normalisedOffset++;
			}
		}

		// NOTE: this 'if' may cause addressStr to be an empty string, if selecting from at the start of rootNode
		if (addressStr.length > 0 || normalisedOffset > 0) {
			// don't put '/0's to the end of an address,
			// to try and make addresses of a specific point unique
			addressStr = '/'+normalisedOffset + addressStr;
		}
		//debug([addressStr, node, offset, str]);
		offset = $(node.parentNode.childNodes).index(node);
		node = node.parentNode;
	}
	while ($(this.rootNode).ancestorOf(node) || this.rootNode == node);
	
	// Defensive programming, never return empty addresses (use to prevent problem as described in above NOTE)!!!
	if (addressStr.length == 0) {
		addressStr = '/0';
	}
	
	return addressStr;
}
/**
 * Get the node-offset represented by addressStr.
 * @param {String} addressStr The address string that represents the location of node-offset, relative to rootNode.
 * @return {Object} The node-offset as properties of the object.
 */
Locator.prototype.getNodeOffsetFromAddress = function(addressStr) {
	var node = document.documentElement;
	var offsets = [];
	var strOffset = 0;
	addressStr = String(addressStr);
	
	// Defensive programming: return rootNode if empty addressStr (assume '' == '/0')
	if (addressStr == '') {
		return {node: this.rootNode, offset: 0};
	}
	
	// Extract the condensed string offset if it exists (after the '.')
	if (addressStr.lastIndexOf('.') > 0) {
		var splits = addressStr.split('.');
		addressStr = splits[0];
		strOffset = Number(splits[1]);
	}
	
	// Prepare offsets
	node = this.rootNode;
	offsets = addressStr.split('/');
	offsets.shift(); // remove the 1st item, which is always an empty string
	
	// Parse through address sequence to find correct node
	for (var i=0; i<offsets.length; i++) {
		var locator = this;
		var prevNode = null;
		var childNodes = $(node.childNodes).filter(function() {
			var curNode = this;
			if (locator.canIgnore(curNode)) {
				return false;
			}
			else if (prevNode && locator.canNormalise(curNode, prevNode)) {
				prevNode = curNode;
				return false;
			}
			else {
				prevNode = curNode;
				return true;
			}
		});
		
		//debug(addressStr+'.'+strOffset, node, childNodes);
		if (childNodes.length <= offsets[i]) {
			// defensive programming: prevent offset from ever overflowing childNodes.length
			return {node: node, offset: node.childNodes.length};
		} 
		else if (i == offsets.length-1 && strOffset == 0) {
			return {node: node, offset: $(node.childNodes).index(childNodes.get(offsets[i]))};
		} 
		else {
			node = childNodes.get(offsets[i]);
		}
	}
	
	// When strOffset > 0, it means that the point:
	// - lies within a text node
	// - within an element with a normalisable class
	// - in between a text node and a normalisable element
	// So we need unnormalise node (and uncondense offset) to find proper node and offset
	if (strOffset > 0) {
		var str = '';
		do {
			// keep shifting node pointer rightwards to nextSibling 
			// while strOffset > (condensed length of str + text of node)
			while (node.nextSibling && strOffset > this.whitespace.condense(this.whitespace.ltrim(str+this.getNodeText(node))).length) {
				str = str + this.getNodeText(node);
				node = node.nextSibling;
			}
			//debug('1', [str, this.getNodeText(node), strOffset]);
			
			// recursively "drill" node pointer downwards to it's firstChild while it has children,
			// and while strOffset <= (condensed length of str + text of node)
			while (node.firstChild && strOffset <= this.whitespace.condense(this.whitespace.ltrim(str+this.getNodeText(node))).length) {
				node = node.firstChild;
			}
			//debug('2', [str, this.getNodeText(node), strOffset]);
		}
		while (node != node.parentNode.lastChild && strOffset > this.whitespace.condense(this.whitespace.ltrim(str+this.getNodeText(node))).length);
		
		var nodeText = this.getNodeText(node);
		var ltrimmed = this.whitespace.ltrim(str+nodeText);
		if (strOffset > ltrimmed.length) strOffset = ltrimmed.length; // improve fault tolerance (in case strOffset, the condensed string offset, overflows)
		
		// actual offset is the offset in the DOM's current state (including highlights, etc., which are not part of the original document)
		var actualOffset = this.whitespace.uncondenseOffset(ltrimmed, strOffset) + (str.length+nodeText.length - ltrimmed.length);
		//debug([(node.data).substring(0, actualOffset-str.length), strOffset, actualOffset, actualOffset - str.length]);
		actualOffset -= str.length;
		if (actualOffset == 0) {
			return {node: node.parentNode, offset: $(node.parentNode.childNodes).index(node)};
		} else {
			return {node: node, offset: actualOffset};
		}
	}
}
/**
 * Get the DOM Range between startAddress and endAddress.
 * @param {String} startAddress Address to the startContainer-startOffset of the Range.
 * @param {String} endAddress Address to the endContainer-endOffset of the Range.
 * @param {Boolean} [snap] If true, the start of the Range will be snapped forward (see {@link Locator#snapForward}), and the end of the Range will be snapped backward (see {@link Locator#snapBackward}).
 * @return {Range} The DOM Range denoted by the given addresses, and snapped where necessary.
 */
Locator.prototype.getRange = function(startAddress, endAddress, snap) {
	var start = this.getNodeOffsetFromAddress(startAddress);
	if (snap) start = this.snapForward(start.node, start.offset);
	var end = this.getNodeOffsetFromAddress(endAddress);
	if (snap) end = this.snapBackward(end.node, end.offset);
	var range = this.rootNode.ownerDocument.createRange();
	range.setStart(start.node, start.offset);
	range.setEnd(end.node, end.offset);
	//debug([start.node, start.offset], [end.node, end.offset], [range.toString()]);
	return range;
}
/**
 * Move the offset forward, until the character at offset is not a whitespace character, or the end of node is reached.
 * @param {Node} node The DOM Text Node offset is in.
 * @param {Number} offset The integer offset.
 * @return {Object} The node and snapped offset as properties of the object.
 */
Locator.prototype.snapForward = function(node, offset) {
	if (node.nodeType == NodeType.TEXT_NODE) {
		offset = this.whitespace.snapForward(node.data, offset);
	}
	return {node:node, offset:offset};
}
/**
 * Move the offset backward, until the character at just before offset is not a whitespace character, or the start of node is reached.
 * @param {Node} node The DOM Text Node offset is in.
 * @param {Number} offset The integer offset.
 * @return {Object} The node and snapped offset as properties of the object.
 */
Locator.prototype.snapBackward = function(node, offset) {
	if (node.nodeType == NodeType.TEXT_NODE) {
		offset = this.whitespace.snapBackward(node.data, offset);
	}
	return {node:node, offset:offset};
}

/**
 * Highlighter class constructor.
 * @class Highlight the text of DOM Ranges.
 * @param {Node} rootNode The root Node of the DOM subtree where highlighting will occur.
 * @param {Object} [options] Set of optional settings.
 * @param {String} [options.highlightClass='h'] HTML class name of highlighting Elements.
 * @param {String} [options.tagName='span'] HTML Tag name of highlighting Elements.
 * @param {String[]} [options.ignoreClasses=[]] List of HTML class names of elements to ignore.
 * @param {String} [options.idPrefix='m'] The prefix for all automatically generated identifiers.
 * @param {String} [options.whitespace=same as {@link Whitespace#constructor}'s default] String of whitespace characters.
 */
Highlighter = function(rootNode, options) {
	this.settings = jQuery.extend({
		highlightClass: 'h',
		ignoreClasses: [],
		tagName: 'span',
		idPrefix: 'm',
		whitespace: undefined
	}, options);
	this.rootNode = rootNode;
	this.events = {};
	this.whitespace = new Whitespace(this.settings.whitespace);
}
/**
 * Highlight the text within range by surrounding each Text node with a highlighting element.
 * @param {Range} range The DOM Range to highlight.
 * @param {String} [id] The unique identifier for this set of highlighting elements.
 * @return {String} id If id is defined,<br />
 *                  An automatically generated unique identifier for this set of highlighting elements otherwise.
 */
Highlighter.prototype.highlight = function(range, id) {
	if (id === undefined) id = ''+this.settings.idPrefix+((new Date()).getTime());
	
	// Create a new, Range object, with start and end collapsed at the start of range
	var startSplicer = document.createRange();
	startSplicer.setStart(range.startContainer, range.startOffset);
	startSplicer.collapse(true);

	// Create another new Range object, with the start and end collapsed at the end of range
	var endSplicer = document.createRange();
	endSplicer.setStart(range.endContainer, range.endOffset);
	endSplicer.collapse(true);

	// Insert a end marker
	var endMarker = document.createElement(this.settings.tagName);
	endSplicer.insertNode(endMarker);

	// Insert a start marker
	var startMarker = document.createElement(this.settings.tagName);
	startSplicer.insertNode(startMarker);

	// At this point, any non-live ranges (e.g. IERange) would have been invalidated,
	// so create a new Range, starting after the start marker, and ending before the end marker
	var newRange = document.createRange();
	newRange.setStartAfter(startMarker);
	newRange.setEndBefore(endMarker);

	// Get all descendant text nodes of newRange.commonAncestorContainer, not within an input or ignorable elements,
	// and filter out those that are not found in newRange, and store the rest into textNodes.
	var highlighter = this;
	var inIgnoreOrInput = ':input *, :input';
	if (this.settings.ignoreClasses.length > 0) {
		inIgnoreOrInput += ', .'+this.settings.ignoreClasses.join(', .');
		inIgnoreOrInput += ', .'+this.settings.ignoreClasses.join(' *, .')+' *';
	}
	var textNodes = $(newRange.commonAncestorContainer).find('*')
					.add(newRange.commonAncestorContainer)
					.not(inIgnoreOrInput)
					.contents().add(newRange.commonAncestorContainer)
					.filter(
		function() {
			if (this.nodeType != NodeType.TEXT_NODE) return false;
			if (highlighter.whitespace.isWhitespace(this.data)) {
				/*if (!highlighter.isHighlighted(this) &&
					!(this.previousSibling && highlighter.isHighlighted(this.previousSibling) &&
					  this.nextSibling && highlighter.isHighlighted(this.nextSibling)) )*/
					return false;
			}
			
			var textRange = document.createRange();
			//textRange.setStart(this, 0);
			//textRange.setEnd(this, this.data.length);
			textRange.selectNode(this);
			
			if (textRange.compareBoundaryPoints(CompareRanges.START_TO_START, newRange) >= 0 &&
				textRange.compareBoundaryPoints(CompareRanges.END_TO_END, newRange) <= 0) {
				return true; // keep
			} else {
				return false; // filter out
			}
		}
	);
	
	// Wrap all selected (text) nodes with the highlight and a (common) unique ID for referencing
	textNodes.wrap('<'+this.settings.tagName+' class="'+this.settings.highlightClass+' '+id+'"></span>');
	
	// Set the data and event handlers for the highlight
	var highlights = $(this.getHighlights(id));
	highlights.data('id', id);
	for (var type in this.events) {
		for (var i in this.events[type]) {
			highlights.bind(type, this.events[type][i]);
		}
	}
	
	// Remove the start and end markers, then cleanup (normalize text nodes)
	var nodesToNormalize = [startMarker.nextSibling, endMarker.nextSibling];
	$(startMarker).remove();
	$(endMarker).remove();
	this.mergeAdjacentText(nodesToNormalize);
	
	return id;
}
/**
 * Remove/unsurround all highlighting elements of the set specified by id from the Text nodes.
 * Adjacent Text nodes are normalised/merged if any exist due to the highlighting elements being removed.
 * @param {String} id The unique identifier for this set of higlighting elements to remove.
 */
Highlighter.prototype.unhighlight = function(id) {
	var highlights = this.getHighlights(id);
	var nodesToNormalize = [];
	highlights.each(function() {
		if (this.firstChild != null) nodesToNormalize.push(this.firstChild);
		if (this.nextSibling != null) nodesToNormalize.push(this.nextSibling);
		$(this).replaceWith(this.childNodes);
	})
	this.mergeAdjacentText(nodesToNormalize);
}
/**
 * Recursively merge the text nodes given with all adjacent their adjacent sibling text nodes.
 * Basically, doing a localised text-node normalisation (to workaround IE6's buggy implementation of Node.normalize()).
 * @param {Nodes[]} nodes The list of nodes to merge; Elements and their children not found in nodes are ignored.
 */
Highlighter.prototype.mergeAdjacentText = function(nodes) {
	for (var i in nodes) {
		// Ignore non-text nodes
		if (nodes[i] == null || nodes[i].nodeType != NodeType.TEXT_NODE) continue;
		
		// Start from the right-most adjacent text node sibling
		var node = nodes[i];
		while (node.nextSibling != null && node.nextSibling.nodeType == NodeType.TEXT_NODE) {
			node = node.nextSibling;
		}
		
		// Traverse through adjacent text node siblings leftwards,
		// storing the combined text of all these adjacent text into str
		var str = node.data;
		var nodesToRemove = [];
		while (node.previousSibling != null && node.previousSibling.nodeType == NodeType.TEXT_NODE) {
			str = node.previousSibling.data + str;
			nodesToRemove.push(node);
			node = node.previousSibling;
		}
		
		// Set the text of the left-most adjacent text node sibling to str,
		// and remove all the adjacent siblings right of it
		node.data = str;
		$(nodesToRemove).remove();
	}
}
/**
 * Bind a live event handler to an Event for all current and future label Elements.
 * @param {String} type The Event type to bind to.
 * @param {Function} fn The handler function.
 */
Highlighter.prototype.bindHighlights = function(type, fn) {
	if (this.events[type] === undefined) {
		this.events[type] = [];
	}
	// only bind function if it's not already binded
	if (jQuery.inArray(fn, this.events[type]) == -1) {
		this.events[type].push(fn);
		$(this.settings.tagName+'.'+this.settings.highlightClass).bind(type, fn);
	}
}
/**
 * Unbind/remove a bound live event handler from all current and future highlighting Elements.
 * @param {String} type The Event type.
 * @param {Function} fn The handler function to unbind.
 */
Highlighter.prototype.unbindHighlights = function(type, fn) {
	if (this.events[type] === undefined) {
		return false;
	}
	for (var i=0; i<this.events[type].length; i++) {
		// take event handler out...
		var temp = this.events[type].shift();
		if (this.events[type][i] != fn) {
			// and put back if it's not the one to unbind
			this.events[type].push(temp);
		} else {
			// or unbind it if it's the one
			$(this.settings.tagName+'.'+this.settings.highlightClass).unbind(type, fn);
		}
	}
}
/**
 * Get the set of highlighting Elements specified by id.
 * @param {String} id Identifier for the set of highlighting Elements.
 * @return {jQuery} The set of highlighting Elements accessible as an Array.
 */
Highlighter.prototype.getHighlights = function(id) {
	return $(this.settings.tagName+'.'+this.settings.highlightClass+'.'+id);
}
/**
 * Get the identifier of the set of highlighting Elements the highlightElement belongs to.
 * @param {Element} highlightElement The highlighting Element.
 * @return {String} The identifier of the set of highlighting Elements highlightElement belongs to.
 */
Highlighter.prototype.getId = function(highlightElement) {
	return $(highlightElement).data('id');
}
/**
 * Check if node is either a highlighting Element itself, or within a highlighting Element.
 * @param {Node} node A DOM Node.
 * @return {Boolean} true If node is highlighted,
 *                   false Otherwise.
 */
Highlighter.prototype.isHighlighted = function(node) {
	return ($(node).closest(this.settings.tagName+'.'+this.settings.highlightClass).length > 0);
}

/**
 * Labeller class constructor.
 * @class Add labels to highlighted Ranges.
 * @param {Object} [options] Set of optional settings.
 * @param {String} [options.labelClass='label'] HTML class name of label Elements.
 * @param {String} [options.containerClass='lc'] HTML class name of label container Elements.
 * @param {String} [options.highlightClass='h'] HTML class name of highlighting Elements.
 * @param {String} [options.whitespace=same as {@link Whitespace#constructor}'s default] String of whitespace characters.
 */
Labeller = function(options) {
	this.settings = jQuery.extend({
		labelClass: 'label',
		containerClass: 'labelcont',
		highlightClass: 'h',
		whitespace: undefined
	}, options);
	this.whitespace = new Whitespace(this.settings.whitespace);
	this.events = {};
}
/**
 * Get the label specified by id.
 * @param {String} id Identifier of the label.
 * @return {Element} The label Element specified by id, if it exists,
					 undefined Otherwise.
 */
Labeller.prototype.getLabel = function(id) {
	return $('.'+this.settings.containerClass+' > .'+this.settings.labelClass+'.'+id)[0];
}
/**
 * Get all labels, sorted in the in-order traversal order (top-to-bottom, left-to-right), as they are found in the DOM tree.
 * @return {Element[]} The set of all label Elements found in the main document accessible as an Array.
 */
Labeller.prototype.getAllLabels = function() {
	return $('.'+this.settings.containerClass+' > .'+this.settings.labelClass);
}
/**
 * Get the unique identifier for labelElement.
 * @param {Element} labelElement The label Element.
 */
Labeller.prototype.getId = function(labelElement) {
	return $(labelElement).data('id');
}
/**
 * Get the label specified by id, if it exists, or create a new label with id as it's identifier for the highlights.
 * @param {String} id The unique identifier for/of the label.
 * @param {Element[]} highlights The list of highlighting elements accessible as an Array.
 * @return {Element} The label Element, either already existing or newly created (if not already existing).
 */
Labeller.prototype.label = function(id, highlights) {
	var label = this.getLabel(id);
	if (label) return label;
	if (!highlights) throw new Error('"highlights" argument is not optional');
	
	// create new label
	label = $('<i class="'+this.settings.labelClass+' '+id+'"></i>')[0];
	$(label).data('id', id);
	
	// Add event handlers for the new label
	for (var type in this.events) {
		for (var i in this.events[type]) {
			$(label).bind(type, this.events[type][i]);
		}
	}
	
	// create new container and put label in it
	var labelContainer = $('<b class="'+this.settings.containerClass+'">WAMP</b>')[0];
	$(highlights[0]).prepend(labelContainer);
	$(labelContainer).empty();
	//$(labelContainer).bind('mouseup mousedown', function(event) { return false; });
	$(labelContainer).prepend(label);
	
	// Find any existing label containers adjacent to any current highlights (i.e. labelling any span within any of the current highlights), 
	// and bring/merge them into the deepest/current highlights
	// Assumes each highlight span can only have ignorable and/or normalisable childNodes (e.g. no <p> or <b> can exist within a highlight)
	// Assumes a newly created label is always within the innermost/deepest highlighting Element
	// Assumes labelContainers are always the first children of highlight spans
	for (var i=0; i<highlights.length; i++) {
		var cur = highlights[i];
		do {
			if (cur.previousSibling && $(cur.previousSibling).hasClass(this.settings.containerClass)) {
				// if there's a label container just before the current span, put it into the innermost part of the current span
				var outerContainer = $(cur.previousSibling);
				if ($(highlights[i].firstChild).hasClass(this.settings.containerClass)) {
					// if there's already a label container within the innermost highlight, merge the inner and outer containers
					var innerContainer = $(highlights[i].firstChild);
					innerContainer.prepend(outerContainer.contents());
					outerContainer.remove();
				}
				else {
					// if not, just put the outerContainer into the innermost highlight
					$(highlights[i]).prepend(outerContainer);
				}
			}
			cur = cur.parentNode;
		} while ($(cur).hasClass(this.settings.highlightClass));
	}
	
	return label;
}
/**
 * Remove the label Element specified by id.
 * @param {String} id The unique identifier of the label to remove.
 */
Labeller.prototype.unlabel = function(id) {
	var label = this.getLabel(id);
	if (label) {
		/**
		 * Remove label and update remaining labels
		 */
		var labelContainer = $(label).closest('.'+this.settings.containerClass);
		$(label).remove();
		
		// move/delete label container as necessary
		if ($(labelContainer).children().length == 0) {
			// delete label container if there's no more labels in it
			$(labelContainer).remove();
		}
	}
}
/**
 * Bind a live event handler to an Event for all current and future label Elements.
 * @param {String} type The Event type to bind to.
 * @param {Function} fn The handler function.
 */
Labeller.prototype.bindLabels = function(type, fn) {
	if (this.events[type] === undefined) {
		this.events[type] = [];
	}
	// only bind function if it's not already binded
	if (jQuery.inArray(fn, this.events[type]) == -1) {
		this.events[type].push(fn);
		$('.'+this.settings.containerClass+' > .'+this.settings.labelClass).bind(type, fn);
	}
}
/**
 * Unbind/remove a bound live event handler from all current and future label Elements.
 * @param {String} type The Event type.
 * @param {Function} fn The handler function to unbind.
 */
Labeller.prototype.unbindLabels = function(type, fn) {
	if (this.events[type] === undefined) {
		return false;
	}
	for (var i=0; i<this.events[type].length; i++) {
		// take event handler out...
		var temp = this.events[type].shift();
		if (this.events[type][i] != fn) {
			// and put back if it's not the one to unbind
			this.events[type].push(temp);
		} else {
			// or unbind it if it's the one
			$('.'+this.settings.containerClass+' > .'+this.settings.labelClass).unbind(type, fn);
		}
	}
}

/**
 * Popup class constructor.
 * @class Create absolutely-positioned popup "windows", with various options.
 * @param {Object} contentElement The content Element for this Popup.
 * @param {Object} [options] Set of optional settings.
 * @param {String} [options.contentClass='popup-content'] HTML class name to add to contentElement.
 * @param {Number|String} [options.x='center'] Default horizontal position (see {@link Popup#setPosition}).
 * @param {Number|String} [options.y='center'] Default vertical position (see {@link Popup#setPosition}).
 * @param {Number} [options.zIndex=1000] Integer CSS z-index of this popup
 * @param {Object} [options.show={params:{opacity:'show'}, duration:100, easing:'linear'}] Animation parameters used when showing this popup (see <a href="http://docs.jquery.com/Effects/animate">arguments of jQuery's animate</a>).
 * @param {Object} [options.hide={params:{opacity:'hide'}, duration:200, easing:'linear'}] Animation parameters used when hiding this popup (see <a href="http://docs.jquery.com/Effects/animate">arguments of jQuery's animate</a>).
 * @param {Boolean} [options.constrainToWindow=true] Determine if this popup should be moved to try to keep within the viewport when showing.
 * @param {Number} [options.overlayOpacity='popup-overlay'] This popup's overlay's opacity (from 0 to 1.0, with 0 meaning no overlay, and 1.0 being a fully opaque overlay).
 * @param {String} [options.overlayClass] HTML class name for this popup's overlay.
 * @param {Object} [options.show={params:{opacity:'show'}, duration:200, easing:'linear'}] Animation parameters used when showing this popup's overlay (see <a href="http://docs.jquery.com/Effects/animate">arguments of jQuery's animate</a>).
 * @param {Object} [options.hide={params:{opacity:'hide'}, duration:200, easing:'linear'}] Animation parameters used when hiding this popup's overlay (see <a href="http://docs.jquery.com/Effects/animate">arguments of jQuery's animate</a>).
 */
Popup = function(contentElement, options) {
	// declare vars
	this.settings = {};
	this.isShowing = false;
	
	this.settings = jQuery.extend(true, {
		contentClass: 'popup-content',
		x: 'center', 
		y: 'center',
		zIndex: 1000,
		show: { params: {opacity:'show'}, duration: 100, easing: 'linear' },
		hide: { params: {opacity:'hide'}, duration: 200, easing: 'linear' },
		constrainToWindow: true,
		overlayOpacity: 0,
		overlayClass: 'popup-overlay',
		overlayShow: { params: {opacity:'show'}, duration: 200, easing: 'linear' },
		overlayHide: { params: {opacity:'hide'}, duration: 200, easing: 'linear' }
	}, options);
	
	// Content element
	this.content = contentElement;
	$(this.content).addClass(this.settings.contentClass);
	$(this.content).css({
		display: 'none',
		position: 'absolute',
		zIndex: this.settings.zIndex
	});
	$(document.body).append(this.content);
	
	// Keep focus in popup
	$(this.content).css({display:'block'});
	var focusables = $(this.content).find('*').add(this.content).filter(':input').filter(':visible');
	$(this.content).css({display:'none'});
	focusables.filter(':first, :last').bind('keydown', function(event) {
		if (event.keyCode != KeyCode.TAB) {
			return;
		}
		
		var first = focusables.filter(':first').get(0);
		var last = focusables.filter(':last').get(0);
		
		if (event.target == first && event.shiftKey) {
			setTimeout(function() {
				$(last).focus();
			}, 1);
			return false;
		}	
		else if (event.target == last && !event.shiftKey) {
			setTimeout(function() {
				$(first).focus();
			}, 1);
			return false;
		}	
	});
	
	// Overlay
	if (this.settings.overlayOpacity > 0) {
		this.overlay = $('<div></div>').get(0);
		$(this.overlay).css({
			position: 'fixed',
			width: '100%',
			height: '100%',
			opacity: this.settings.overlayOpacity,
			top: 0,
			left: 0,
			zIndex: this.settings.zIndex,
			display: 'none'
		});
		$(this.overlay).addClass(this.settings.overlayClass);
		$(document.body).append(this.overlay);
	}
	
	// Detect IE6 and below, and apply hacks
	if (typeof document.body.style.maxHeight === 'undefined') {
		if (this.settings.overlayOpacity > 0) {
			// IE6 Hack to position: fixed
			var popup = this;
			var updateOverlay = function(event) {
				$(popup.overlay).css({
					position: 'absolute',
					width: $(window).width(),
					height: $(window).height(),
					top: $(document).scrollTop(),
					left: $(document).scrollLeft()
				});
			}
			$(window).resize(updateOverlay); 
			$(window).scroll(updateOverlay);
		}
	}
}
/**
 * Set the position of this Popup's content Element.<br />
 * The positions can either be in integer pixel coordinates, or the String: "center".<br />
 * All positions are relative to the top-left pixel of the viewport.<br />
 * If the position is not specified, the default position specified in the constructor settings will be used instead.
 * @param {Number|String} x The new left position relative to the viewport (optional).
 * @param {Number|String} y The new top position relative to the viewport (optional).
 */
Popup.prototype.setPosition = function(x, y) {
	if (x !== undefined) this.settings.x = x;
	if (y !== undefined) this.settings.y = y;
	
	var height = $(this.content).outerHeight(true);
	switch (this.settings.y) {
		case 'top': break;
		case 'center': 
			y = ($(window).height()/2)-(height/2);
		break;
		case 'bottom': break;
		default: 
			y = this.settings.y;
	}
	if (this.settings.constrainToWindow) {
		if (y < 0) y = 0;
		else if (y+height >= $(window).height()) y = $(window).height()-height;
	}
	
	var width = $(this.content).outerWidth(true);
	switch (this.settings.x) {
		case 'left': break;
		case 'center':
			x = ($(window).width()/2)-(width/2);
		break;
		case 'right': break;
		default: 
			x = this.settings.x;
	}
	if (this.settings.constrainToWindow) {
		if (x < 0) x = 0;
		else if (x+width >= $(window).width()) x = $(window).width()-width;
	}
	
	$(this.content).css({
		top: $(window).scrollTop()+y,
		left: $(window).scrollLeft()+x
	});
}
/**
 * Show this Popup at the specified location, or at it's default location if not specified.<br />
 * The overlay will also appear below the Popup if it's opacity is greater than zero.
 * @event
 * @param {Object} [x] The new left position (see {@link Popup#setPosition}).
 * @param {Object} [y] The new left position (see {@link Popup#setPosition}).
 */
Popup.prototype.show = function(x, y) {
	this.setPosition(x, y);
	if (this.isShowing == false) {
		var anim;
		if (this.settings.overlayOpacity > 0) {
			anim = this.settings.overlayShow;
			$(document.body).append(this.overlay);
			$(this.overlay).animate(anim.params, anim.duration, anim.easing, anim.callback);
		}
		anim = this.settings.show;
		$(document.body).append(this.content);
		$(this.content).animate(anim.params, anim.duration, anim.easing, anim.callback);
		this.isShowing = true;
	}
	$(this).triggerHandler('show');
}
/**
 * Hide this Popup and it's overlay.
 * @event
 */
Popup.prototype.hide = function() {
	if (this.isShowing) {
		var anim = this.settings.hide;
		$(this.content).animate(anim.params, anim.duration, anim.easing, anim.callback);
		if (this.settings.overlayOpacity > 0) {
			anim = this.settings.overlayHide;
			$(this.overlay).animate(anim.params, anim.duration, anim.easing);
		}
		this.isShowing = false;
	}
	$(this).triggerHandler('hide');
}

/**
 * Annotations class constructor.
 * @class Load, manage, and save annotations in XML format.
 * @param {String} url URL address of the annotations XML file to load from.
 */
Annotations = function(url) {
	this.url = url;
	this.defaultXMLString = '<annotations></annotations>';
	this.xml = XMLTools.stringToXML(this.defaultXMLString);
}
/**
 * Event fired when {@link Annotations#load} has completed successfully.
 * @name Annotations#loadsuccess
 * @event
 * @param {Event} event
 */
/**
 * Event fired when {@link Annotations#load} has failed, either because of problems connecting to the URL, or the returned XML is not well-formed.
 * @name Annotations#loadfailure
 * @event
 * @param {Event} event
 * @param {Error} event.errorThrown The error that caused the failure.
 */
/**
 * Load annotations from url if specified, or the default url otherwise (as specified in the constructor).
 * @param {String} [url] URL address of the annotations XML file to load from.
 * @return {XMLHttpRequest} The XMLHttpRequest object created for the operation.
 *							null If there was an error creating the object.
 */
Annotations.prototype.load = function(url) {
	if (url === undefined) url = this.url;
	var anno = this;
	var xhr = jQuery.ajax({
		url: url,
		type: 'POST',
		processData: true,
		dataType: 'text',
		data: {
			operation:'load'
		},
		success: function(data, textStatus) {
			try {
				if (data.length == 0) data = anno.defaultXMLString;
				var tempXML = XMLTools.stringToXML(data);
				if ($(tempXML).find('parsererror').length > 0) throw new Error($(tempXML).find('parsererror')[0].firstChild.data);
				anno.xml = tempXML;
				$(anno).trigger('loadsuccess');
			}
			catch (e) {
				$(anno).trigger({
					type: 'loadfailure',
					errorThrown: e
				});
				return null;
			}
		},
		error: function(XMLHttpRequest, textStatus, errorThrown) {
			$(anno).trigger({
				type: 'loadfailure',
				XMLHttpRequest: XMLHttpRequest,
				textStatus: textStatus, 
				errorThrown: errorThrown
			});
			//anno.xml = XMLTools.stringToXML(anno.defaultXMLString);
		}
	});
	return xhr;
}
/**
 * Event fired when {@link Annotations#save} has completed successfully.
 * @name Annotations#savesuccess
 * @event
 * @param {Event} event
 */
/**
 * Event fired when {@link Annotations#save} has failed.
 * @name Annotations#savefailure
 * @event
 * @param {Event} event
 * @param {Error} event.errorThrown The error that caused the failure.
 */
/**
 * Save annotations to url if specified, or the default url otherwise (as specified in the constructor).
 * @param {String} [url] URL address to send the XML representation of this set of Annotations.
 * @return {XMLHttpRequest} The XMLHttpRequest object created for the operation.
 *							null If there was an error creating the object.
 */
Annotations.prototype.save = function(url) {
	if (url === undefined) url = this.url;
	var anno = this;
	//debug(XMLTools.xmlToString($(anno.xml).children('annotations').get(0)));
	var xhr = jQuery.ajax({
		url: url,
		type: 'POST',
		processData: true,
		dataType: 'text',
		data: {
			operation:'save', 
			xml:XMLTools.xmlToString($(anno.xml).children('annotations').get(0))
		},
		success: function(data) {
			try {
				debug(XMLTools.xmlToString($(anno.xml).children('annotations').get(0)));
				//if (data != XMLTools.xmlToString($(anno.xml).children('annotations').get(0))) throw new Error('Reply from server not the same as data sent; possible corruption while sending'); 
				$(anno).trigger('savesuccess');
			}
			catch (e) {
				$(anno).trigger({
					type: 'savefailure',
					errorThrown: e
				});
				debug(e);
				return null;
			}
		},
		error: function(xhr, textStatus, errorThrown) {
			$(anno).trigger({
				type: 'savefailure',
				XMLHttpRequest: XMLHttpRequest,
				textStatus: textStatus, 
				errorThrown: errorThrown
			});
			//debug(textStatus, errorThrown);
		}
	});
	return xhr;
}
/**
 * Set the parameters of the mistake specified by id, overwriting the first/only existing mistake with the same id if found.
 * @param {String} id Identifier for the mistake.
 * @param {Object} params Parameters of the mistake.
 * @param {String} params.start Starting location of mistake (see {@link Locator}).
 * @param {String} params.end Ending location of mistake (see {@link Locator}).
 * @param {String} params.type Type of mistake.
 * @param {String} params.correction Correction for mistake.
 * @param {String} params.comments Comments on mistake.
 * @param {String} [params.text] Original/Source text of mistake.
 * @return {Object} The mistake information object (same format as params in {@link Annotations#setMistake}) of the overwritten mistake (with the same id).
 *					null If no mistake was overwritten.
 */
Annotations.prototype.setMistake = function(params) {
	if (params.id===undefined || params.start===undefined || params.end===undefined) throw new Error('id/start/end not found');
	
	var mistake = this.xml.createElement('mistake');
	$(mistake).attr('id', params.id);
	$(mistake).attr('start', params.start);
	$(mistake).attr('end', params.end);
	
	if (!params.type) throw new Error('Mistake type not found');
	var type = this.xml.createElement('type');
	$(type).text(params.type);
	$(mistake).append(type);
	
	var correction = this.xml.createElement('correction');
	$(correction).text(params.correction);
	$(mistake).append(correction);

	if (params.type == 'Others' && !params.comments) throw new Error('Mistakes of type "Others" must have comments');
	var comments = this.xml.createElement('comments');
	$(comments).text(params.comments);
	$(mistake).append(comments);
	
	var orig = this.getMistake(params.id);
	if (orig) {
		var elementToReplace = $(this.xml).find('mistake#'+params.id).get(0);
		elementToReplace.parentNode.replaceChild(mistake, elementToReplace); // cannot use jQuery.replace() with IE's XML object... 
		return orig;
	}
	else {
		$(this.xml).children('annotations').eq(0).append(mistake);
		return null;
	}
}
/**
 * Get the mistake specified by id.
 * @param {String} id Identifier of the mistake.
 * @return {Object} The mistake information object (same format as params in {@link Annotations#setMistake}) if found.
 *                  null Otherwise.
 */
Annotations.prototype.getMistake = function(id) {
	var mistake = $(this.xml).find('mistake#'+id);
	
	if (mistake.length == 0) return null;
	else return {
		id: id,
		start: mistake.attr('start'),
		end: mistake.attr('end'),
		type: mistake.find('type').text(),
		correction: mistake.find('correction').text(),
		comments: mistake.find('comments').text()
	};
}
/**
 * Get all mistakes in this Annotations set.
 * @return {Object[]} All mistakes' information objects (same format as params in {@link Annotations#setMistake}).
 */
Annotations.prototype.getAllMistakes = function(/*start*/) {
	var anno = this;
	var list = [];
	$(this.xml).find('mistake').each(function() {
		// filter out those that don't match the search params
		//if (start && start != $(this).attr('start')) return;
		
		// push those that match onto the list
		list.push(anno.getMistake($(this).attr('id')));
	});
	return list;
}
/**
 * Remove the mistake from this Annotations set.
 * @param {String} id The identifier of the mistake to remove.
 * @return {Object} The removed mistake's information object (same format as params in {@link Annotations#setMistake}).
 */
Annotations.prototype.removeMistake = function(id) {
	var elementToRemove = $(this.xml).find('mistake#'+id).get(0);
	if (elementToRemove) {
		var mistake = this.getMistake(id);
		elementToRemove.parentNode.removeChild(elementToRemove); // cannot use jQuery.remove() with IE's XML object... 
		return mistake;
	}
	else {
		throw new Error('mistake#'+id+' not found');
	}
}
/**
 * Remove all mistakes from this Annotations set.
 */
Annotations.prototype.removeAllMistakes = function() {
	$(this.xml).find('mistake').remove();
}