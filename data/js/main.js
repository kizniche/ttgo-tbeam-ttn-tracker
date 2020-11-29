//-- Custom components -------------------------------------------------------------------------------------------------

/**
 * Simple card element containing a 'title' and a 'content' slot
 */
customElements.define('ui-card', class Card extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplCard').content;
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
        this.hasUnsavedChanges = false;
        const btn = this.shadowRoot.getElementById('btnCommit');
    }
});

/**
 * Styled checkbox component
 */
customElements.define('ui-checkbox', class extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplCheckbox').content;
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
        this.checkbox = this.shadowRoot.getElementById('check');
        this.checkbox.addEventListener('change', (ev) => {
            const myEvent = new ev.constructor(ev.type, ev);
            this.dispatchEvent(myEvent);
        });
    }

    get checked() {
        return this.checkbox.checked;
    }

    set checked(value) {
        this.checkbox.checked = value;
    }
});

/**
 * Log data display
 */
customElements.define('ui-log', class extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplLogContainer').content;
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
        this.logContainer = this.shadowRoot.getElementById('logContainer');
        this.logEventSource = null;
    }

    clearLog() { }

    addLogEntry(text) {
        const entry = document.createElement('ui-log-entry');
        const eContent = document.createElement('span');
        entry.appendChild(eContent);
        eContent.textContent = text;
        eContent.setAttribute('slot', 'content');
        this.logContainer.appendChild(entry);
        return entry;
    }

    connectedCallback() {
        this.logEventSource = new EventSource('/sse/log');
        this.logEventSource.addEventListener('log', (ev) => {
            this.addLogEntry(ev.data);
        });
    }

    disconnectedCallback() {
        if (this.logEventSource) {
            this.logEventSource.close();
            this.logEventSource = null;
        }
    }
}
);

/**
 * Component representing a single log entry
 */
customElements.define('ui-log-entry', class extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplLogEntry').content;
        const shadowRoot = this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
    }
}
);

/**
 * A simple dialog popup including a backdrop
 */
customElements.define('ui-dialog', class DialogUI extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplDialog').content;
        // No shadow DOM here
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
        this.close = this.close.bind(this);
    }

    close() {
        this.dispatchEvent(new Event('closed'));
        this.parentNode.removeChild(this);
    }

    connectedCallback() {
        let type = this.getAttribute('data-type');
        if (['error', 'info', 'warn'].indexOf(type) == -1) {
            type = '';
        }
        if (type != '') {
            this.shadowRoot.querySelector('dialog').classList.add(type);
        }
        this.querySelectorAll('button').forEach((item) => {
            item.addEventListener('click', this.close);
        })
    }
});

/**
 * Full-screen loading spinner based on
 * https://projects.lukehaas.me/css-loaders/#load7
 */
customElements.define('ui-spinner', class SpinnerUI extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplSpinner').content;
        // No shadow DOM here
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
    }

    show() {
        this.shadowRoot.querySelector('.spinner').classList.add('active');
    }

    hide() {
        this.shadowRoot.querySelector('.spinner').classList.remove('active');
    }
});

/**
 * Configuration form used for setting the values
 */
customElements.define('ui-config', class ConfigUi extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplConfigUi').content;
        // No shadow DOM here
        this.appendChild(
            tpl.cloneNode(true)
        );
        this.hasUnsavedChanges = false;
        this.saveChanges = this.saveChanges.bind(this);
        this.setFormElementStates = this.setFormElementStates.bind(this);
        // Bind change events to all of our inputs
        this.querySelectorAll('input,select,ui-checkbox').forEach((item) => {
            item.addEventListener('change', () => this.setUnsavedChanges(true));
        })
        this.querySelectorAll('ui-checkbox').forEach((item) => {
            item.addEventListener('change', () => this.setFormElementStates());
        });
    }

    showDialog(dialogType, title, message, buttons) {
        const diag = document.createElement('ui-dialog');
        diag.setAttribute('data-type', dialogType);
        let span = document.createElement('span');
        span.setAttribute('slot', 'title');
        span.textContent = title;
        diag.appendChild(span);
        span = document.createElement('span');
        span.setAttribute('slot', 'content');
        if (message instanceof HTMLElement) {
            span.appendChild(message);
        } else {
            span.textContent = message;
        }
        diag.appendChild(span);
        span = document.createElement('span');
        span.setAttribute('slot', 'actions');
        if (Array.isArray(buttons)) {
            buttons.forEach((item) => {
                if (item instanceof HTMLElement) {
                    span.appendChild(item);
                } else {
                    // Can be used for buttons that will only close the dialog
                    const btn = document.createElement('button');
                    btn.innerText = item;
                    span.appendChild(btn);
                }
            });
        }
        diag.appendChild(span);
        this.appendChild(diag);
        return diag;
    }

    setUnsavedChanges(value) {
        this.hasUnsavedChanges = value;
        const fn = this.hasUnsavedChanges ? DOMTokenList.prototype.add : DOMTokenList.prototype.remove;
        const btn = document.getElementById('btnSave');
        fn.call(btn.classList, 'active');
    }

    showSpinner() {
        this.querySelector('ui-spinner').show();
    }

    hideSpinner() {
        this.querySelector('ui-spinner').hide();
    }

    setFormElementStates() {
        let enabled = this.querySelector('#sleepBetweenMessages')?.checked || false;
        this.querySelector('#sleepDelay').disabled = !enabled;
        enabled = this.querySelector('#apEnabled')?.checked || false;
        this.querySelector('#ssid').disabled = !enabled;
        this.querySelector('#psk').disabled = !enabled;
    }

    /**
     * Loads the given config data into the form.
     * T-Beam's configuration comes in this format:
     *
     * {
     *      lora: {
     *          port: 1,                        // LoRa Port to send the message with
     *          sf: 0,                          // Spread Factor to use when sending (0 = SF12 - 5 = SF7)
     *          useAdr: false,                  // Should ADR be used?
     *          confirmedEvery: 0               // Send a confirmed message every N messages - 0 means never
     *      },
     *      wifi: {
     *          enabled: true,                  // Should WiFi be enabled on boot? Can be changed by pressing the select button
     *          ssid: "MySSID",                 // The SSID to use for the created AccessPoint
     *          psk: "MyWiFiPassword"           // The PSK for the created AccessPoint
     *      },
     *      system: {
     *          interval: 15000,
     *          sleepBetweenMessages: false,    // Should the ESP32 go to sleep between messages?
     *          sleepDelay: 5000                // Delay to wait until going to sleep after sending a message
     *      }
     * }
     *
     * @param {Object} data The data loaded from the T-Beam
     */
    refreshFormElements(data) {
        this.querySelector('#interval').value = data?.system?.interval || '15000';
        this.querySelector('#sleepDelay').value = data?.system?.sleepDelay || '5000';
        this.querySelector('#sleepBetweenMessages').checked = Boolean(data?.system?.sleepBetweenMessages);

        this.querySelector('#sf').value = data?.lora?.sf || '5';
        this.querySelector('#port').value = data?.lora?.port || '1';
        this.querySelector('#adr').checked = Boolean(data?.lora?.useAdr);
        this.querySelector('#confirmedEvery').value = Number(data?.lora?.confirmedEvery) || 0;

        this.querySelector('#apEnabled').checked = Boolean(data?.wifi?.enabled);
        this.querySelector('#ssid').value = data?.wifi?.ssid || '';
        this.querySelector('#psk').value = data?.wifi?.psk || '';
        this.setFormElementStates();
    }

    showError(title, message, err) {
        const el = document.createElement('span');
        el.innerText = message;
        const pre = document.createElement('pre');
        if (err?.message && err?.name) {
            pre.innerText = `${err.name}\n${err.message}`;
        } else {
            pre.innerText = err;
        }
        el.appendChild(pre);
        this.showDialog('error', title, el, ['Ok']);
    }

    loadSettings() {
        this.showSpinner();
        fetch('/api/config').then(async (body) => {
            let data = null;
            try {
                data = await body.json();
            } catch (err) {
                this.showError('Error loading configuration', 'Failed to parse response body:', err);
            }
            this.refreshFormElements(data);
        }).catch((err) => {
            this.showError('Error loading configuration', 'Failed to load configuration data:', err);
        }).finally(() => {
            this.hideSpinner();
        })
    }

    saveChanges() {
        if (!this.validateConfigData()) {
            return;
        }
        const el = document.createElement('span');
        el.innerHTML = `Saving the settings will restart the device.<p>
                <i>Depending on if you changed the WiFi settings, you might need to re-join with the new settings.</i>
            </p>`;
        const saveBtn = document.createElement('button');
        saveBtn.innerText = 'Save and restart...';
        saveBtn.addEventListener('click', () => {
            this.commitConfigData();
        });
        this.showDialog(
            'info',
            'Information',
            el,
            [saveBtn, 'Cancel']
        );
    }

    validateConfigData() {
        // Right now only SSID and PSK need to be checked here
        const ssid = this.querySelector('#ssid').value;
        const psk = this.querySelector('#psk').value;
        if (ssid.length === 0 || ssid.length > 32) {
            this.showError(
                'Invalid configuration',
                'The wifi SSID is invalid',
                'Please enter a SSID with 1 to 32 characters'
            );
            return false;
        }
        if (psk.length < 8 || psk.length > 63) {
            this.showError(
                'Invalid configuration',
                'The wifi password is invalid',
                'Please use a password with 8 to 63 characters'
            );
            return false;
        }
        return true;
    }

    /**
     * Reads the config data from the form fields and sends it to the board
     */
    commitConfigData() {
        // Second time - just to be sure - we'll not reach here if the first one fails, but this will
        // block any direct call of circumventing this check
        if (!this.validateConfigData()) {
            return;
        }
        const conf = {
            system: {
                interval: Number.parseInt(this.querySelector('#interval')?.value) || '15000',
                sleepDelay: Number.parseInt(this.querySelector('#sleepDelay')?.value) || '5000',
                sleepBetweenMessages: Boolean(this.querySelector('#sleepBetweenMessages')?.checked)
            },
            lora: {
                sf: Number.parseInt(this.querySelector('#sf')?.value) || '5',
                port: Number.parseInt(this.querySelector('#port')?.value) || '1',
                useAdr: Boolean(this.querySelector('#adr')?.checked),
                confirmedEvery: Number.parseInt(this.querySelector('#confirmedEvery')?.value) || 0
            },
            wifi: {
                enabled: Boolean(this.querySelector('#apEnabled')?.checked),
                ssid: this.querySelector('#ssid')?.value || '',
                psk: this.querySelector('#psk')?.value || ''
            }
        };
        console.dir(conf);
        const hdr = new Headers();
        hdr.set('Content-Type', 'application/json');
        fetch('/api/config', {
            method: 'POST',
            body: JSON.stringify(conf),
            headers: hdr
        }).then(async (res) => {
            if (res.ok) {
                this.showDialog(
                    'info',
                    'Config saved',
                    `The configuration has been written successfully.
                        Please reload this page once the device connection has been re-established.`,
                    []);
                this.setUnsavedChanges(false);
            } else {
                // Try to decode the error payload
                try {
                    const payload = await res.json();
                    const err = payload?.error || payload;
                    this.showError(
                        `Error (${res.status})`,
                        `Saving the configuration has failed:`,
                        err
                    );
                } catch (err) {
                    this.showError(
                        `Error (${res.status})`,
                        `Saving the configuration has failed.
                            There was also an error decoding the error response
                            (HTTP ${res.status} - ${res.statusText}):`,
                        err
                    );
                }
            }
        }).catch((err) => {
            this.showError(
                'Error sending request',
                'Sending the configuration data to the device has failed:',
                err
            )
        });
    }

    connectedCallback() {
        document.getElementById('btnSave').addEventListener('click', this.saveChanges);
        this.loadSettings();
    }

    disconnectedCallback() {
        this.setUnsavedChanges(false)
        document.getElementById('btnSave').removeEventListener('click', this.saveChanges);
    }

});

//-- Global functionality ----------------------------------------------------------------------------------------------

const availableTargets = ['status', 'log', 'config'];

const nav = (target) => {
    if (availableTargets.indexOf(target) == -1) {
        window.location.hash = '#config'; // Nav to main page
        return;
    }
    // Load the main content
    const main = document.getElementById('content');
    main.innerHTML = '';
    const tplName =
        'tplMain' + target.charAt(0).toUpperCase() + target.substr(1);
    const tpl = document.getElementById(tplName);
    if (tpl) {
        main.appendChild(tpl.content.cloneNode(true));
    } else {
        console.error(`Failed to find template '${tplName}' in DOM.`);
        main.innerHTML =
            '<ui-card><span slot="title">Error</span><span slot="content">Location template not found';
    }
    // Set the correct menu item
    const items = document.getElementById('mainNav').children[0].children;
    Array.prototype.forEach.call(items, (item) => {
        const targetUrl = new URL(item.children[0].href);
        if (targetUrl.hash.substr(1) == target) {
            item.classList.add('selected');
        } else {
            item.classList.remove('selected');
        }
    });
};

window.onhashchange = () => nav(window.location.hash.substr(1));
