/**
 * @fileOverview This file defines the WAMP Annotator User Interface classes, which emulate the classical single-inheritance model.
 *               <br />Current class hierarchy: (fundamental ancestor) BaseUI -> PrintUI -> ViewUI -> EditUI (highest-level descendant).
 *				 <br />(i.e. EditUI, in addition to editing functionality, also has printing, viewing and highlighting/labelling functionality inherited from all it's ancestor classes).
 * @author Pua Jun Hong
 */

/**
 * Convenience function to easily extend a Function object from it's Parent, 
 * which copies all Parent's instance members to this Function's prototype, 
 * and add's a reference of that Parent's prototype to the _super instance variable in this Function.
 * <br />Make sure this method is called immediately after your constructor definition, but before you extend the prototype for the object.
 * @param {Function} Parent Parent Function object to extend/inherit from.
 */
Function.prototype.inherits = function(Parent) {
	this.prototype = new Parent();
	this.prototype.constructor = this;
	this._super = Parent.prototype;
}

/**
 * BaseUI class constructor.
 * @class The base user interface class, which loads and prepares all UI Elements for use by subclasses, loads the annotations, and marks (highlights and labels) the mistakes in the specified content body.
 * @param {String} uiURL The HTML file containing all the HTML user interface Elements.
 * @param {String} annotationsURL The XML file containing all the annotations (format same as that accepted by the Annotations class).
 * @param {Object} [elemIds] The set of Element identifiers for the content body, and all user interface Elements.
 * @param {String} [elemIds.body='wamp_body'] Identifier of the content body.
 * @param {String} [elemIds.table='wamp_table'] Identifier of the table of annotation details (see {@link PrintUI}).
 * @param {String} [elemIds.status='wamp_status'] Identifier of the loading status popup.
 * @param {String} [elemIds.list='wamp_list'] Identifier of the list of currently selected/hovered annotations (see {@link ViewUI}).
 * @param {String} [elemIds.view='wamp_view'] Identifier of the annotation details view (see {@link ViewUI}).
 * @param {String} [elemIds.menu='wamp_menu'] Identifier of the selection context menu (see {@link EditUI}).
 * @param {String} [elemIds.form='wamp_form'] Identifier of the form for editing/creating annotations (see {@link EditUI}).
 * @param {Object} [classes] The class names of various Elements created by the user interface.
 * @param {String} [classes.highlight='h'] Class name of highlighting Elements.
 * @param {String} [classes.active='active'] Class name of the active highlighting (and label) Elements (see {@link ViewUI}).
 * @param {String} [classes.hover='hover'] Class name of the highlighting Element being hovered/selected (see {@link ViewUI}).
 * @param {String} [classes.labelContainer='lc'] Class name Elements that contains labels.
 * @param {String} [classes.label='l'] Class name of label Elements.
 */
BaseUI = function(uiURL, annotationsURL, elemIds, classes) {
	var ui = this;
	
	// Compulsory settings
	ui.uiURL = uiURL;
	ui.annotationsURL = annotationsURL;
	
	// Optional settings (with defaults)
	ui.elemIds = jQuery.extend({
		body: 'wamp_body',
		table: 'wamp_table',
		status: 'wamp_status',
		list: 'wamp_list',
		view: 'wamp_view',
		menu: 'wamp_menu',
		form: 'wamp_form'
	}, elemIds);
	ui.classes = jQuery.extend({
		highlight: 'h',
		active: 'active',
		hover: 'hover',
		labelContainer: 'lc',
		label: 'l'
	}, classes);
	
	// Global vars
	ui.lastSelectedRange = null;
	ui.activeHighlight = {};
	ui.hoverHighlights = {};
	
	// UI elements
	ui.body = null;
	ui.status = null;
	ui.menu = null;
	ui.list = null;
	ui.form = null;
	ui.view = null;
	
	// Helper objects
	ui.annotations = null;
	ui.whitespace = new Whitespace();
	ui.locator = null;
	ui.highlighter = null;
	ui.labeller = null;
	
	ui.loadUI(ui.uiURL);
}
BaseUI.prototype.loadUI = function(url) {
	var ui = this;
	if (url) ui.uiURL = url;
	else url = ui.uiURL;
	
	var temp = $('<div id="wamp_temp"></div>');
	temp.load(url, function (responseText, textStatus, XMLHttpRequest) {
		temp.children().appendTo(document.body);
		ui.loadUIComplete();
	});
}
BaseUI.prototype.loadUIComplete = function() {
	var ui = this;
	
	// Setup UI elements
	ui.setupBody($('#'+ui.elemIds.body).get(0));
	ui.setupTable($('#'+ui.elemIds.table).get(0));
	ui.setupStatus($('#'+ui.elemIds.status).get(0));
	ui.setupMenu($('#'+ui.elemIds.menu).get(0));
	ui.setupForm($('#'+ui.elemIds.form).get(0));
	ui.setupList($('#'+ui.elemIds.list).get(0));
	ui.setupView($('#'+ui.elemIds.view).get(0));
	
	// Setup utility objects (they require ui.body, so must be done after setupBody())
	ui.setupLocator(ui.body, { 
		normaliseClasses: [ui.classes.highlight, '__mozilla-findbar-search'],
		ignoreClasses: [ui.classes.labelContainer]
	});
	ui.setupHighlighter(ui.body, {
		highlightClass: ui.classes.highlight,
		ignoreClasses: [ui.classes.labelContainer],
		idPrefix: 'm'
	});
	ui.setupLabeller({
		containerClass: ui.classes.labelContainer,
		labelClass: ui.classes.label,
		highlightClass: ui.classes.highlight
	});
	
	// Load annotations only after everything has been set up
	ui.loadAnnotations(ui.annotationsURL);
}
BaseUI.prototype.loadAnnotations = function(url) {
    var ui = this;
	if (url) ui.annotationsURL = url;
	else url = ui.annotationsURL;
	
	// Show status
	$(ui.status.content).text('Loading annotations, please wait...');
	ui.status.show($('#wamp_body').offset().left-$(window).scrollLeft(), $('#wamp_body').offset().top-$(window).scrollTop());

	ui.annotations = new Annotations(url);
	ui.annotations.load();
	$(ui.annotations).bind('loadsuccess', function(event) {
		ui.loadAnnotationsComplete();
	});
	$(ui.annotations).bind('loadfailure', function(event) {
		ui.status.show($('#wamp_body').offset().left-$(window).scrollLeft(), $('#wamp_body').offset().top-$(window).scrollTop());
		$(ui.status.content).text('Loading failed (>_<). What to do?');
		debug(event.errorThrown);
	});
}
BaseUI.prototype.loadAnnotationsComplete = function() {
	var ui = this;
	var mistakes = ui.annotations.getAllMistakes();
	
	// Parse mistakes serially and non-blockingly, releasing the processor after each mistake
	// Actually slower, but perceptually faster
	var initDelay = 100, interDelay = 1;
	var time = (new Date()).getTime();
	(function() {
		// Use closures to keep track of counter i
		var i=0;
		function parseMistake() {
			if (mistakes.length > 0) {
				ui.markMistake(mistakes[i]);
				i++;
			}
			if (i < mistakes.length) {
				setTimeout(parseMistake, interDelay);
			}
			else {
				debug('total time: ', (new Date()).getTime()-time-initDelay-(mistakes.length*interDelay));
				ui.status.hide();
			}
		}
		setTimeout(parseMistake, initDelay);
	})();
	
	/*
	// Parse mistakes, blocking/freezing the browser until everything is done
	var time = (new Date()).getTime();
	for (var i=0; i<mistakes.length; i++) {
		ui.markMistake(mistakes[i]);
	}
	debug('total time: ', (new Date()).getTime()-time);
	*/
}
BaseUI.prototype.setupLocator = function(bodyElement, options) {
	var ui = this;
	ui.locator = new Locator(bodyElement, options);
}
BaseUI.prototype.setupHighlighter = function(bodyElement, options) {
	var ui = this;
	ui.highlighter = new Highlighter(bodyElement, options);
}
BaseUI.prototype.setupLabeller = function(options) {
	var ui = this;
	ui.labeller = new Labeller(options);
}
BaseUI.prototype.setupBody = function(bodyElement) {
	var ui = this;
	ui.body = bodyElement;
}
BaseUI.prototype.setupTable = function(tableElement) {
	var ui = this;
	ui.table = tableElement;
	$(ui.table).remove();
}
BaseUI.prototype.setupStatus = function(statusElement) {
	var ui = this;
	ui.status = new Popup(statusElement, {
		constrainToWindow: false,
		overlayOpacity: 0.8
	});
	$(ui.status).bind('show', function(event) {
		$(window).triggerHandler('resize');
	});
	$(window).bind('resize', function(event) {
		$(ui.status.overlay).css({
			position: 'absolute',
			background: '#666666',
			left: $('#wamp_body').offset().left,
			top: $('#wamp_body').offset().top,
			width: $('#wamp_body').outerWidth(false),
			height: $('#wamp_body').outerHeight(false)
		});
	});
}
BaseUI.prototype.setupMenu = function(menuElement) {
	var ui = this;
	ui.menu = new Popup(menuElement);
}
BaseUI.prototype.setupForm = function(formElement) {
	var ui = this;
	ui.form = new Popup(formElement, {
		overlayOpacity: 0.5,
		constrainToWindow: false
	});
}
BaseUI.prototype.setupView = function(viewElement) {
	var ui = this;
	ui.view = new Popup(viewElement, {
		overlayOpacity: 0.5,
		contrainToWindow: false
	});
}
BaseUI.prototype.setupList = function(listElement) {
	var ui = this;
	ui.list = new Popup(listElement);
}
/**
 * Highlight and label the annotated mistake in the content body.
 */
BaseUI.prototype.markMistake = function(mistake) {
	var ui = this;
	
	/**
	 * Update highlights
	 */
	var highlights = ui.highlighter.getHighlights(mistake.id);
	if (highlights.length == 0) {
		// highlight annotated range if not highlighted yet
		var range = ui.locator.getRange(mistake.start, mistake.end);
		ui.highlighter.highlight(range, mistake.id);
		highlights = ui.highlighter.getHighlights(mistake.id);
	}
	// remove active class (if it exists)
	highlights.removeClass(ui.classes.active);
	
	/**
	 * Update/Create label as necessary
	 */
	var label = $(ui.labeller.label(mistake.id, highlights));
	
	// Just change the text of the existing label for this mistake (since structure of DOM not changed; nothing added, nothing removed)
	label.text(mistake.type);
	//label.html('<i class="index"></i><i class="type">'+mistake.type+'</i>');
	
	// Remove active highlights
	label.removeClass(ui.classes.active);	
}
/**
 * Remove highlighting and labels of the annotated range specified by mistakeId.
 */
BaseUI.prototype.unmarkMistake = function(mistakeId) {
	var ui = this;
	
	// Remove label (if it exists)
	ui.labeller.unlabel(mistakeId);
	
	// Remove highlights last (so it can clean up and normalise the text nodes)
	ui.highlighter.unhighlight(mistakeId);
}

/**
 * PrintUI class constructor.
 * @extends BaseUI
 * @class Add printing table of annotations and indexes to labels, for easier referencing of annotations in printed output.
 * @param {String} [elemIds.table] Identifier of the table of annotations (shown when printing).
 */
PrintUI = function(uiURL, annotationsURL, elemSelectors, classes) {
	// Call super constructor
	PrintUI._super.constructor.call(this, uiURL, annotationsURL, elemSelectors, classes);
}
PrintUI.inherits(BaseUI); // PrintUI extends BaseUI
/**
 * Add the table of annotation details to the document body.
 */
PrintUI.prototype.setupTable = function(tableElement) {
	// Call super function
	PrintUI._super.setupTable.call(this, tableElement);
	
	var ui = this;
	$(document.body).append(ui.table);
}
/**
 * Add index to label of mistake, and an entry of mistake details into the table of details.
 */
PrintUI.prototype.markMistake = function(mistake) {
	// Call super function
	PrintUI._super.markMistake.call(this, mistake);
	
	var ui = this;
	var label = $(ui.labeller.getLabel(mistake.id));
	label.html('<i class="index"></i><i class="type">'+mistake.type+'</i>');
	
	/**
	 * Update table row
	 */
	var row = $(ui.table).find('tbody tr.'+mistake.id);
	if (row.length == 0) {
		row = $('<tr class="'+mistake.id+'"]> <td></td> <td></td> <td></td> <td></td> </tr>');
		$(ui.table).find('tbody').append(row);
	}
	row.children('td:eq(1)').text(mistake.type);
	row.children('td:eq(2)').text(mistake.correction);
	row.children('td:eq(3)').text(mistake.comments);
	
	/**
	 * Update indexes and sort rows
	 */
	var labels = ui.labeller.getAllLabels(); 
	var index = labels.index(label);
	row.siblings(':eq('+index+')').before(row);
	var rows = $(ui.table).find('tbody tr');
	for (var i=index; i<labels.length; i++) {
		labels.eq(i).children(':eq(0)').text((i+1)+'.');
		rows.eq(i).children(':eq(0)').text((i+1)+'.');
	}
}
/**
 * Update indexes of remaining annotation labels and table entries.
 */
PrintUI.prototype.unmarkMistake = function(mistakeId) {
	var ui = this;
	
	// Update indexes
	var label = ui.labeller.getLabel(mistakeId);
	var labels = ui.labeller.getAllLabels(); 
	var index = labels.index(label);
	for (var i=index+1; i<labels.length; i++) {
		var id = ui.labeller.getId(labels[i]);
		$(ui.table).find('tbody tr.'+id+' td:first').text(i+'.');
		$(ui.labeller.getLabel(id)).children(':first').text(i+'.');
	}
	
	// Remove table row
	$(ui.table).find('tbody tr.'+mistakeId).remove();
	
	// Call super function after indexes are updated
	PrintUI._super.unmarkMistake.call(this, mistakeId);
}

/**
 * ViewUI class constructor.
 * @extends PrintUI
 * @class Add the functionality to select and view details of annotations.
 * @param {String} [elemIds.list] Identifier of the list of currently selected/hovered annotations (shown when the the user clicks on any annotated range).
 * @param {String} [elemIds.view] Identifier of the annotation details view (shown when the user selects from the list of currently selected/hovered annotations).
 */
ViewUI = function(uiURL, annotationsURL, elemSelectors, classes) {
	// Call super constructor
	ViewUI._super.constructor.call(this, uiURL, annotationsURL, elemSelectors, classes);
}
ViewUI.inherits(PrintUI); // ViewUI extends PrintUI
/**
 * Add events to highlights (hover, click).
 */
ViewUI.prototype.setupHighlighter = function(bodyElement, options) {
	// Call super function
	ViewUI._super.setupHighlighter.call(this, bodyElement, options);
	
	var ui = this;
	ui.highlighter.bindHighlights('mouseover', function(event) {
		var id = ui.highlighter.getId(this);
		$(ui.highlighter.getHighlights(id)).addClass(ui.classes.hover);
		$(ui.labeller.getLabel(id)).addClass(ui.classes.hover);
		ui.hoverHighlights[id] = ui.highlighter.getHighlights(id);
		return true;
	});
	ui.highlighter.bindHighlights('mouseout', function(event) {
		var id = ui.highlighter.getId(this);
		$(ui.highlighter.getHighlights(id)).removeClass(ui.classes.hover);
		$(ui.labeller.getLabel(id)).removeClass(ui.classes.hover);
		delete ui.hoverHighlights[id];
		return true;
	});
	ui.highlighter.bindHighlights('click', function(event) {
		if (ui.whitespace.trim(window.getSelection().toString()).length > 0) {
			ui.list.hide();
			return true;
		} else {
			var id = ui.highlighter.getId(this);
			if (ui.highlighter.isHighlighted(this.parentNode) == false || event.isPropagationStopped()) {
				// make only one highlight open the ui.list
				ui.list.show(event.pageX-$(window).scrollLeft(), event.pageY-$(window).scrollTop());
				return false;
			} else {
				return true;
			}
		}
	});
}
/**
 * Add events to labels (hover, click).
 */
ViewUI.prototype.setupLabeller = function(options) {
	// Call super function
	ViewUI._super.setupLabeller.call(this, options);
	
	var ui = this;
	// Live-bind events to labels
	ui.labeller.bindLabels('mouseover mouseout click', function(event) {
		var thisId = ui.labeller.getId(this);
		for (var id in ui.hoverHighlights) {
			if (id != thisId) {
				$(ui.highlighter.getHighlights(id)[0]).trigger('mouseout');
			}
		}
		event.stopPropagation();
		$(ui.highlighter.getHighlights(thisId)[0]).trigger(event);
	});
}
/**
 * Hide the list of currently selected/hovered annotations when clicking anywhere else on the viewport.
 */
ViewUI.prototype.setupBody = function(bodyElement) {
	// Call super function
	ViewUI._super.setupBody.call(this, bodyElement);
	
	var ui = this;
	$(document).bind('mousedown keydown', function(event) {
		ui.list.hide();
	});
}
/**
 * Setup details view (populate details, cancel button).
 */
ViewUI.prototype.setupView = function(viewElement) {
	// Call super function
	ViewUI._super.setupView.call(this, viewElement);
	
	var ui = this;
	$(ui.view).bind('show', function(event) {
		var mistake = ui.annotations.getMistake(ui.activeHighlight.id);
		$('#wamp_view_type').text($('#wamp_form_type option[value="'+mistake.type+'"]').text());
		$('#wamp_view_correction').text(mistake.correction);
		$('#wamp_view_comments').text(mistake.comments);
		
		// Enable inputs
		$(ui.view.content).find(':input').removeAttr('disabled');
		
		ui.activateMistake(mistake.id);
		$(ui.view.content).find(':button[name="cancel"]').focus();
		$(ui.view.content).find('.message').text('');
	});
	
	$(ui.view.content).find(':button').hide(0).bind('click', function(event) {
		event.preventDefault();
		if ($(this).attr('disabled')) {
			event.stopImmediatePropagation();
		} else {
			// Disable inputs
			$(ui.view.content).find(':input').attr('disabled', 'disabled');
		}
	});
	
	$(ui.view.content).find(':button[name="cancel"]').show(0).bind('click', function(event) {
		// Close details view
		ui.deactivateMistake(ui.activeHighlight.id);
		ui.view.hide();
	});
	$(ui.view.content).bind('mousedown', function(event) {
		$(this).startDrag(event);
	});
	$(ui.view.content).bind('mouseup', function(event) {
		$(this).stopDrag();
	});
	$(ui.view.content).find(':input, dd').bind('mousedown', function(event) {
		event.stopPropagation();
	});
	$(ui.view.overlay).bind('click', function(event) {
		$(ui.view.content).find(':button[name="cancel"]').trigger('click');
	});
	$(ui.view.overlay).bind('keypress', function(event) {
		var action = event.keyCode;
		switch (action) {
			case KeyCode.ESCAPE:
				$(ui.view.content).find(':button[name="cancel"]').trigger('click');
			break;
		}
	});
}
/**
 * Setup events for the list of currently selected/hovered annotations (click, hover).
 */
ViewUI.prototype.setupList = function(listElement) {
	// Call super function
	ViewUI._super.setupList.call(this, listElement);
	
	var ui = this;
	$(ui.list.content).bind('click mousedown', function(event) {
		event.stopPropagation(); // stop the menu from jumping around, which makes the user have to chase after the menu
	});
	$(ui.list).bind('show', function(event) {
		// Create list of ui.annotations to select from
		// Only re-create every opening, so that the mouse can go elsewhere without affect the position and contents of the list
		$(ui.list.content).empty();
		for (var id in ui.hoverHighlights) {
			var mistake = ui.annotations.getMistake(id);
			debug(mistake);
			var item = $('<div></div>');
			item.append($('<b>'+$('#wamp_form_type option[value="'+mistake.type+'"]').text()+'</b>'));
			item.append('<br />');
			item.append('<span>'+mistake.correction+'</span>');
			item.bind('mouseenter', function(event) {
				$(ui.labeller.getLabel($(this).data('id'))).addClass(ui.classes.hover);
				$(this).addClass(ui.classes.hover);
				$(ui.highlighter.getHighlights($(this).data('id'))).addClass(ui.classes.hover);
			});
			item.bind('mouseleave', function(event) {
				$(ui.labeller.getLabel($(this).data('id'))).removeClass(ui.classes.hover);
				$(this).removeClass(ui.classes.hover);
				$(ui.highlighter.getHighlights($(this).data('id'))).removeClass(ui.classes.hover);
			});
			item.data('id', id);
			item.bind('click', function(event) {
				var id = $(this).data('id');
				$(ui.labeller.getLabel(id)).removeClass(ui.classes.hover);
				$(ui.highlighter.getHighlights(id)).removeClass(ui.classes.hover);
				var mistake = ui.annotations.getMistake(id);
				ui.activeHighlight = {
					id: id,
					start: mistake.start,
					end: mistake.end
				};
				ui.view.show();
				ui.list.hide();
			});
			$(ui.list.content).append(item);
		}
	});
}
/**
 * Emphasize the annotated range specified by mistakeId.
 */
ViewUI.prototype.activateMistake = function(mistakeId) {
	var ui = this;
	$(ui.highlighter.getHighlights(mistakeId)).addClass(ui.classes.active);
	$(ui.labeller.getLabel(mistakeId)).addClass(ui.classes.active);
	//$('.'+ui.classes.labelContainer+' *:not(#'+mistakeId+')').hide();
}
/**
 * Deemphasize the annotated range specified by mistakeId.
 */
ViewUI.prototype.deactivateMistake = function(mistakeId) {
	var ui = this;
	$(ui.highlighter.getHighlights(mistakeId)).removeClass(ui.classes.active);
	$(ui.labeller.getLabel(mistakeId)).removeClass(ui.classes.active);
	//$('.'+ui.classes.labelContainer+' *:not(#'+mistakeId+')').show();
}

/**
 * EditUI class constructor.
 * @extends ViewUI
 * @class Add the functionality to create new annotations, and delete existing annotations.
 * @param {String} [elemIds.menu] Identifier of the context menu (shown when a range of text in the content body has been selected).
 * @param {String} [elemIds.form] Identifier of the form for editing/creating annotations (shown when the user chooses to annotate the selected range of text from the context menu).
 */
EditUI = function(uiURL, annotationsURL, elemSelectors, classes) {
	// Call super constructor
	EditUI._super.constructor.call(this, uiURL, annotationsURL, elemSelectors, classes);
}
EditUI.inherits(ViewUI); // EditUI extends ViewUI
/**
 * Setup mouse selection event handlers (react to the selection of text ranges by user).
 */
EditUI.prototype.setupBody = function(bodyElement) {
	// Call super function
	EditUI._super.setupBody.call(this, bodyElement);
	
	var ui = this;
	$(ui.body).bind('mouseup', function(event) {
		// ignore mouseup in form elements
		if ($(event.target).filter(':input, :input *').length > 0) return true;
		
		if (ui.whitespace.trim(window.getSelection().toString()).length > 0) {
			ui.lastSelectedRange = window.getSelection().getRangeAt(0);

			// typeof(window.external.AddService) == 'unknown', which is neither true nor false!!!
			// see: http://robertnyman.com/2005/12/21/what-is-typeof-unknown/
			if (window.external && typeof(window.external.AddService) != 'undefined') {
				// avoid IE8's stupid Accelerator button
				ui.menu.show(event.pageX-$(window).scrollLeft(), event.pageY-$(window).scrollTop()-$(ui.menu.content).outerHeight(true));
			} else {
				ui.menu.show(event.pageX-$(window).scrollLeft(), event.pageY-$(window).scrollTop());
			}
		}
	});
	$(document).bind('mousedown keydown', function(event) {
		ui.menu.hide();
	});
}
/**
 * Setup event handlers for the selection context menu (click, hover, conversion of the selected range into addresses).
 */
EditUI.prototype.setupMenu = function(menuElement) {
	// Call super function
	EditUI._super.setupMenu.call(this, menuElement);
	
	var ui = this;
	$(ui.menu.content).bind('click mousedown', function(event) {
		event.stopPropagation(); // stop the menu from jumping around, which makes the user have to chase after the menu
	});
	$(ui.menu).bind('show', function(event) {
		ui.list.hide(); // make sure only 1 menu (context menu or list menu) appears at a time
	});
	$(ui.menu.content).find('a').bind('click', function(event) {
		var href = $(this).attr('href');
		var action = href.substring(href.indexOf('#')+1);
		//debug(href, action, href.indexOf('#'));
		switch (action) {
			case 'annotate':
				// save the current selected region and temporarily highlight it
				//ui.lastSelectedRange = window.getSelection().getRangeAt(0);
				if (ui.lastSelectedRange) {
					//var range = window.getSelection().getRangeAt(0);
					var range = ui.lastSelectedRange;
					
					// Snap/shrink to non-whitespace characters
					var start = ui.locator.snapForward(range.startContainer, range.startOffset);
					var end = ui.locator.snapBackward(range.endContainer, range.endOffset);
					
					var startAddress = ui.locator.getAddressFromNodeOffset(start.node, start.offset);
					var endAddress = ui.locator.getAddressFromNodeOffset(end.node, end.offset);
					// Remake range from addresses, so that the highlighted region is exactly the same as the region represented by the addresses stored
					range = ui.locator.getRange(startAddress, endAddress);
					debug([startAddress, endAddress]);
					
					// Get highlighted text
					var id = ui.highlighter.highlight(range);
					var text = '';
					$(ui.highlighter.getHighlights(id)).each(function() {
						text += ui.whitespace.trim(ui.whitespace.condense($(this).text()));
					});
					
					// Only get to annotate if there's actually highlighted (non-whitespace) text
					if (text.length > 0) {
						ui.activeHighlight = {
							id: id,
							start: startAddress,
							end: endAddress
						};
						ui.form.show();
					}
					else {
						ui.highlighter.unhighlight(id);
					}
				}
			break;
			case 'cancel':
				// do nothing
			break;
		}
		
		ui.menu.hide();
		window.getSelection().removeAllRanges();
		return false;
	});
}
/**
 * Setup event handlers for annotation creating/editing form (populate form fields if editing existing annotation, save and cancel buttons).
 */
EditUI.prototype.setupForm = function(formElement) {
	// Call super function
	EditUI._super.setupForm.call(this, formElement);
	
	var ui = this;
	$(ui.form).bind('show', function(event) {
		var mistake = ui.annotations.getMistake(ui.activeHighlight.id);
		
		if (mistake) {
			$(ui.form.content).find(':input[name="type"]').val(mistake.type);
			$(ui.form.content).find(':input[name="correction"]').val(mistake.correction);
			$(ui.form.content).find(':input[name="comments"]').val(mistake.comments);
		} else {
			$(ui.form.content).find('form').get(0).reset();
		}
		
		$(ui.form.content).data('backup', mistake);
		
		// Enable inputs
		$(ui.form.content).find(':input').removeAttr('disabled');
		
		ui.activateMistake(ui.activeHighlight.id);
		$(ui.form.content).find(':input[name="type"]').focus();
		$(ui.form.content).find('.message').text('');
	});
	$(ui.form).bind('hide', function(event) {
		$(ui.form.content).removeData('backup'); 
	});
	$(ui.form.content).find(':button').bind('click', function(event) {
		event.preventDefault();
		if ($(this).attr('disabled')) return;
		// Disable inputs
		$(ui.form.content).find(':input').attr('disabled', 'disabled');
		$(ui.form.content).find('.message').text('');
				
		var action = $(this).attr('name');
		switch (action) {
			case 'save':
				var origMistake = null;
				try {
					// Edit/Create mistake
					origMistake = ui.annotations.setMistake({
						id: ui.activeHighlight.id,
						start: ui.activeHighlight.start,
						end: ui.activeHighlight.end,
						type: $(ui.form.content).find(':input[name="type"]').val(),
						correction: $(ui.form.content).find(':input[name="correction"]').val(),
						comments: $(ui.form.content).find(':input[name="comments"]').val()
					});
					debug(ui.annotations.getMistake(ui.activeHighlight.id));
				}
				catch (e) {
					// Enable inputs
					$(ui.form.content).find(':input').removeAttr('disabled');
					$(ui.form.content).find('.message').text(e.message);
					return;
				}
					
				// Save after each edit/create
				var successHandler = function(event) {
					// Save successfully, so close the form
					ui.markMistake(ui.annotations.getMistake(ui.activeHighlight.id));
					ui.form.hide();
					$(ui.annotations).unbind('savesuccess', successHandler);
					$(ui.annotations).unbind('savefailure', failureHandler);
				};
				var failureHandler = function(event) {
					// Undo changes to annotations
					if (origMistake) {
						ui.annotations.setMistake(origMistake);
					} else {
						ui.annotations.removeMistake(ui.activeHighlight.id);
					}
					// Alert the user
					if (event.errorThrown && event.errorThrown.message) {
						$(ui.form.content).find('.message').text(event.errorThrown.message);
					} else {
						$(ui.form.content).find('.message').text('Error saving to server.');
					}
					//$(ui.form.content).find(':input').removeAttr('disabled');
					$(ui.annotations).unbind('savesuccess', successHandler);
					$(ui.annotations).unbind('savefailure', failureHandler);
				}
				$(ui.annotations).bind('savesuccess', successHandler);
				$(ui.annotations).bind('savefailure', failureHandler);
				ui.annotations.save();
			break;
			case 'cancel':
				if ($(ui.form.content).data('backup')) {
					ui.deactivateMistake(ui.activeHighlight.id);
				} else {
					ui.highlighter.unhighlight(ui.activeHighlight.id);
				}
				ui.form.hide();
			break;
		}
		return false;
	});
	$(ui.form.content).bind('mousedown', function(event) {
		$(this).startDrag(event);
	});
	$(ui.form.content).bind('mouseup', function(event) {
		$(this).stopDrag();
	});
	$(ui.form.content).find(':input').bind('mousedown', function(event) {
		event.stopPropagation();
	});
	$(ui.form.overlay).bind('click', function(event) {
		$(ui.form.content).find(':button[name="cancel"]').trigger('click');
	});
	$(ui.form.overlay).bind('keypress', function(event) {
		var action = event.keyCode;
		switch (action) {
			case 27: // Esc key
				$(ui.form.content).find(':button[name="cancel"]').trigger('click');
			break;
		}
	});
}
/**
 * Add Edit and Delete buttons to details view.
 */
EditUI.prototype.setupView = function(viewElement) {
	// Call super function
	EditUI._super.setupView.call(this, viewElement);
	
	var ui = this;
	$(ui.view.content).find(':button[name="edit"]').show(0).bind('click', function(event) {
		// Open edit form
		ui.view.hide();
		ui.form.show();
	});
	$(ui.view.content).find(':button[name="delete"]').show(0).bind('click', function(event) {
		// Delete and save
		var origMistake = ui.annotations.removeMistake(ui.activeHighlight.id);
		var successHandler = function(event) {
			ui.unmarkMistake(ui.activeHighlight.id);
			ui.view.hide();
			$(ui.annotations).unbind('savesuccess', successHandler);
			$(ui.annotations).unbind('savefailure', failureHandler);
		};
		var failureHandler = function(event) {
			// Undo changes to annotations
			if (origMistake) {
				ui.annotations.setMistake(origMistake);
			}
			// Alert the user
			if (event.errorThrown && event.errorThrown.message) {
				$(ui.view.content).find('.message').text(event.errorThrown.message);
			} else {
				$(ui.view.content).find('.message').text('Error saving to server.');
			}
			//$(ui.view.content).find(':input').removeAttr('disabled');
			$(ui.annotations).unbind('savesuccess', successHandler);
			$(ui.annotations).unbind('savefailure', failureHandler);
		}
		$(ui.annotations).bind('savesuccess', successHandler);
		$(ui.annotations).bind('savefailure', failureHandler);
		ui.annotations.save();
	});
}
