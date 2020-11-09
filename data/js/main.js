// Simple card element containing a 'title' and a 'content' slot
class Card extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplCard').content;
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
        this.hasUnsavedChanges = false;
        const btn = this.shadowRoot.getElementById('btnCommit');
        btn.addEventListener('click', this.saveChanges.bind(this));
        setTimeout(() => {
            this.setUnsavedChanges(true);
        }, 5000);
    }

    setUnsavedChanges(value) {
        if (this.hasUnsavedChanges != value) {
            this.hasUnsavedChanges = value;
            const me = this.shadowRoot.getElementById('main');
            const fn = value ? me.classList.add : me.classList.remove;
            fn.call(me.classList, 'unsavedChanges');
        }
    }

    saveChanges() {
        // Implemented by sub-classes
        this.setUnsavedChanges(false);
    }

}

class LoraConfigCard extends Card {
    constructor() {
        super();
        const tpl = document.getElementById('tplCardLoraConfig').content;
        this.appendChild(tpl.cloneNode(true));
    }

    saveChanges() {
        super.saveChanges();
    }
}


customElements.define('ui-card', Card);
customElements.define('ui-card-lora', LoraConfigCard);

customElements.define('ui-checkbox', class extends HTMLElement {
    constructor() {
        super();
        const tpl = document.getElementById('tplCheckbox').content;
        this.attachShadow({ mode: 'open' }).appendChild(
            tpl.cloneNode(true)
        );
    }
});

customElements.define(
    'ui-log',
    class extends HTMLElement {
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

customElements.define(
    'ui-log-entry',
    class extends HTMLElement {
        constructor() {
            super();
            const tpl = document.getElementById('tplLogEntry').content;
            const shadowRoot = this.attachShadow({ mode: 'open' }).appendChild(
                tpl.cloneNode(true)
            );
        }
    }
);

const availableTargets = ['status', 'log', 'config'];

const nav = (target) => {
    if (availableTargets.indexOf(target) == -1) {
        window.location.hash = '#status'; // Nav to main page
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
