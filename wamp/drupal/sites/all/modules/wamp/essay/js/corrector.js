/**
 * @fileOverview This file defines the Corrector class used to display a corrected version of an annotated content body.
 * @author Pua Jun Hong
 */

 /**
 * Corrector class constructor.
 * @class Replace each annotated mistake in a content body with the suggested correction of that annotation.
 * @param {String} annotationsURL URL address of the annotations XML file to load all the annotations from (file has the same format as that accepted by the Annotations class).
 * @param {Object} [options] Set of optional settings.
 * @param {String} [option.bodyId='wamp_body'] The identifier of the content body Element.
 * @param {String} [option.correctionClass='correction'] The class name of the correction Elements that will replace the original annotated texts.
 */
Corrector = function(annotationsURL, options) {
	var cor = this;
	
	// Compulsory settings
	cor.annotationsURL = annotationsURL;
	
	// Optional settings
	cor.settings = jQuery.extend({
		bodyId: 'wamp_body',
		correctionClass: 'correction'
	});
	
	// UI elements
	cor.body = $('#'+cor.settings.bodyId)[0];
	
	// Helper objects
	cor.annotations = null;
	cor.locator = new Locator(cor.body);
	
	cor.loadAnnotations(cor.annotationsURL);
}
Corrector.prototype.loadAnnotations = function(url) {
	var cor = this;
	
	cor.annotations = new Annotations(url);
	cor.annotations.load();
	$(cor.annotations).bind('loadsuccess', function(event) {
		cor.loadAnnotationsComplete();
	});
}
Corrector.prototype.loadAnnotationsComplete = function() {
	var cor = this;
	var mistakes = cor.annotations.getAllMistakes();
	mistakes.sort(function(mistakeA, mistakeB) {
		return cor.compareAddress(mistakeA.end, mistakeB.end);
	});

	// Parse mistakes serially and non-blockingly, releasing the processor after each mistake
	// Actually slower, but perceptually faster
	var initDelay = 100, interDelay = 1;
	var time = (new Date()).getTime();
	(function() {
		// Use closures to keep track of counter i
		var i=mistakes.length-1;
		var lastStart = null;	
		function parseMistake() {
			if (i >= 0) {
				if (!(lastStart && cor.compareAddress(mistakes[i].end, lastStart) > 0)) {
					cor.correct(mistakes[i]);
					lastStart = mistakes[i].start;
				}
				i--;
				setTimeout(parseMistake, interDelay);
			}
			else {
				debug('total time: ', (new Date()).getTime()-time-initDelay-(mistakes.length*interDelay));
			}
		}
		setTimeout(parseMistake, initDelay);
	})();
	
	/*
	var time = (new Date()).getTime();
	var lastStart = null;	
	for (var i=mistakes.length-1; i>=0; i--) {
		if (!(lastStart && cor.compareAddress(mistakes[i].end, lastStart) > 0)) {
			cor.correct(mistakes[i]);
			lastStart = mistakes[i].start;
		}
	}
	debug('total time: ', (new Date()).getTime()-time);
	*/
}
Corrector.prototype.compareAddress = function(addrA, addrB) {
	var a = addrA.split(/[\/.]/);
	a.shift();
	var b = addrB.split(/[\/.]/);
	b.shift();
	
	// Find least no. of offsets
	var len = (a.length > b.length) ? b.length : a.length;
	
	// Compare each offset
	for (var i=0; i<len; i++) {
		if (Number(a[i]) != Number(b[i])) {
			return Number(a[i]) - Number(b[i]);
		}
	}
	
	// The first len offsets are common
	return a.length - b.length;
}
Corrector.prototype.correct = function(mistake) {
	var cor = this;
	var range = cor.locator.getRange(mistake.start, mistake.end, true);
	//debug(mistake);
	var origStr = cor.getOriginalString(range).replace(/\s+/g, ' ');
	debug([range.startOffset, range.endOffset, origStr]);
	range.extractContents();
	var trimmed = mistake.correction.replace(/^\s+/, '').replace(/\s+$/, '');
	if (trimmed == '') trimmed = '&nbsp;';
	range.insertNode($('<b class="'+cor.settings.correctionClass+'" title="'+origStr+'">'+trimmed+'</b>')[0]);
}
Corrector.prototype.getOriginalString = function(range) {
	var selection = window.getSelection();
	
	// Backup current selection
	var backup = [];
	for (var i=0; i<selection.rangeCount; i++) {
		backup.push(selection.getRangeAt(i));
	}
	
	// Get string
	selection.removeAllRanges();
	selection.addRange(range);
	var str = selection.toString();
	
	// Restore backup
	selection.removeAllRanges();
	for (var i=0; i<backup.length; i++) {
		selection.addRange(backup[i]);
	}
	
	return str;
}