/**
 * @fileOverview This file defines the Converter static class used by the Converter WSH script to convert WAMP-exported XML files into the desired format.
 * @author Pua Jun Hong
 */
 
/**
 * Converter static class.
 * @class Convert XML exported from WAMP into the desired format.
 */
Converter = {}
/**
 * {Boolean} Flag which indicates when {@link Converter#convert} has completed, and whether {@link Converter#output} is ready to be read.
 * true If completed and ready, false Otherwise.
 */
Converter.ok = false;
/**
 * {String} String representation of the resultant, converted XML document, usable only after {@link Converter#convert} has completed, and {@link Converter#ok} is true.
 */
Converter.output = '';
/**
 * Convert the XML file into the desired XML format.
 * When completed, Converter.ok will be set to true, and Converter.output will be set to the string representation of the desired XML.
 * @param {String} url URL address of the XML file to convert.
 * @param {Function} [callback] Function to call when the conversion is complete.
 */
Converter.convert = function(url, callback) {
	var me = this;
	me.whitespace = new Whitespace();
	
	var xmlDoc = XMLTools.loadXMLDoc(url);
	
	var initDelay = 0, interDelay = 1;
	var time = (new Date()).getTime();
	me.ok = false;
	(function() {
		var instances = $(xmlDoc).find('instance');
		var i=0;
		$('#status').text(i+'/'+instances.length+' instances done.');
		
		function doInstance() {
			if (i < instances.length) {
				me.convertInstance(instances[i]);
				i++;
				$('#status').empty();
				$('#status').append(i+'/'+instances.length+' instances done.');
				$('#status').append($('<br />'));
				$('#status').append('Time taken: '+( ((new Date()).getTime()-time-initDelay-(instances.length*interDelay))/1000 )+' seconds');
				setTimeout(doInstance, interDelay);
			}
			else {
				me.ok = true;
				me.output = XMLTools.xmlToString(xmlDoc);
				if (callback) callback(XMLTools.xmlToString(xmlDoc));
			}
		}
		setTimeout(doInstance, initDelay);
	})();
}
Converter.convertInstance = function(instance) {
	var me = this;
	
	var context = $(instance).find('context')[0];
	var clone = $('#wamp_body')[0];
	var html = '';
	for (var i=0; i<context.childNodes.length; i++) {
		html += XMLTools.xmlToString(context.childNodes[i]);
	}
	$(clone).html(html);
	$(document.body).append(clone);
	//console.dirxml(clone);
	me.locator = new Locator(clone);
	var annotationSets = $(instance).find('annotation');
	for (var i=0; i<annotationSets.length; i++) {
		me.convertAnnotationSet(annotationSets[i], clone);
	}
	var range = document.createRange();
	range.selectNode(clone);
	
	// Context format conversion
	context.parentNode.replaceChild(XMLTools.stringToXML(
		'<context><paragraph>'+
		me.splitParas(range).join('</paragraph><paragraph>')+
		'</paragraph></context>'
	).firstChild, context);
}
Converter.convertAnnotationSet = function(annotations, context) {
	var me = this;
	
	var mistakes = $(annotations).find('mistake');
	for (var i=0; i<mistakes.length; i++) {
		me.convertMistake(mistakes[i], context);
	}
}
Converter.convertMistake = function(mistake, context) {
	var me = this;
	var snappedRange = me.locator.getRange($(mistake).attr('start'), $(mistake).attr('end'), true);
	var range = document.createRange();
	range.setStart(context, 0);
	
	// Start paragraph-offset identifier
	range.setEnd(snappedRange.startContainer, snappedRange.startOffset);
	var startParas = me.splitParas(range);
	var startChars = me.whitespace.ltrim(me.whitespace.condense(startParas[startParas.length-1]));
	//$(mistake).attr('start', '/'+startParas.length+'.'+(startChars.length+1));
	mistake.removeAttribute('start');
	mistake.setAttribute('startParagraph', startParas.length-1);
	mistake.setAttribute('startOffset', startChars.length);
	
	// End paragraph-offset identifier
	range.setEnd(snappedRange.endContainer, snappedRange.endOffset);
	var endParas = me.splitParas(range);
	var endChars = me.whitespace.ltrim(me.whitespace.condense(endParas[endParas.length-1]));
	//$(mistake).attr('end', '/'+endParas.length+'.'+(endChars.length+1));
	mistake.removeAttribute('end');
	mistake.setAttribute('endParagraph', endParas.length-1);
	mistake.setAttribute('endOffset', endChars.length);
	
	var correction = $(mistake).find('correction')[0];
	$(correction).text(me.whitespace.trim($(correction).text()));
	
	/*var original = mistake.ownerDocument.createElement('original');
	$(original).text(me.splitParas(snappedRange).join('\n'));
	mistake.appendChild(original);*/
}
Converter.splitParas = function(range) {
	var selection = window.getSelection();
	selection.removeAllRanges();
	selection.addRange(range);
	var str = selection.toString();
	selection.removeAllRanges();
	var arr = str.split(/[\r\n]+(?=.+)/);
	return arr;
}