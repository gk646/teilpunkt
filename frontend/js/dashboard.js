// Sample file system data for simulation
const fileSystem = [
    {
        name: 'Home',
        files: [
            {
                type: 'folder',
                name: 'Documents',
                children: [
                    { type: 'file', name: 'Resume.docx' },
                    { type: 'file', name: 'Budget.xlsx' },
                ]
            },
            {
                type: 'folder',
                name: 'Pictures',
                children: [
                    { type: 'file', name: 'Vacation.jpg' },
                    { type: 'file', name: 'Family.png' },
                ]
            },
            { type: 'file', name: 'todo.txt' }
        ]
    },
    {
        name: 'Shared',
        files: [
            {
                type: 'folder',
                name: 'Projects',
                children: [
                    { type: 'file', name: 'Project1.zip' },
                    { type: 'file', name: 'Project2.zip' },
                ]
            },
            { type: 'file', name: 'readme.md' }
        ]
    }
];

// Navigation state (stack of folders)
let navigationStack = [];

// DOM elements
const sideMenu = document.getElementById('sideMenu');
const mainContent = document.getElementById('mainContent');
const profileButton = document.getElementById('profileButton');

// Initialize UI on page load
document.addEventListener('DOMContentLoaded', () => {
    renderSideMenu();
    loadRoot(fileSystem[0]);
    attachProfileHandler();
});

// Render side menu with available roots
function renderSideMenu() {
    const ul = document.createElement('ul');
    fileSystem.forEach((root, index) => {
        const li = document.createElement('li');
        li.textContent = root.name;
        li.addEventListener('click', () => {
            ul.querySelectorAll('li').forEach(item => item.classList.remove('active'));
            li.classList.add('active');
            navigationStack = [];
            loadFiles(root.files, [root.name]);
        });
        if (index === 0) li.classList.add('active');
        ul.appendChild(li);
    });
    sideMenu.innerHTML = '';
    sideMenu.appendChild(ul);
}

// Load files (or folder contents) into main content
function loadFiles(files, path) {
    navigationStack = path;
    renderBreadcrumb();

    const fileBrowser = document.createElement('div');
    fileBrowser.className = 'file-browser';

    files.forEach(item => {
        const fileItem = document.createElement('div');
        fileItem.className = 'file-item';
        fileItem.addEventListener('click', (e) => {
            // Prevent row click if an action button is clicked
            if(e.target.closest('.file-action')) return;
            handleItemClick(item, path);
        });

        // Modern file icon using inline SVG
        const iconContainer = document.createElement('div');
        iconContainer.className = 'file-icon';
        iconContainer.innerHTML = getIconHTML(item.type);

        // File name
        const fileName = document.createElement('div');
        fileName.className = 'file-name';
        fileName.textContent = item.name;

        // File actions container (options and share)
        const actionsContainer = document.createElement('div');
        actionsContainer.className = 'file-actions';

        // Options button (three dots)
        const optionsButton = document.createElement('button');
        optionsButton.className = 'file-action';
        optionsButton.title = 'Options';
        optionsButton.innerHTML = `<svg viewBox="0 0 24 24" width="16" height="16" fill="var(--text)">
                                 <circle cx="5" cy="12" r="2"/>
                                 <circle cx="12" cy="12" r="2"/>
                                 <circle cx="19" cy="12" r="2"/>
                               </svg>`;
        optionsButton.addEventListener('click', (e) => {
            e.stopPropagation();
            alert(`Options for ${item.name}`);
        });

        // Share button (chain icon)
        const shareButton = document.createElement('button');
        shareButton.className = 'file-action';
        shareButton.title = 'Share';
        shareButton.innerHTML = `<svg viewBox="0 0 24 24" width="16" height="16" fill="var(--text)">
                                <path d="M3.9 12c0-1.1.9-2 2-2h3v2H5.9a.9.9 0 00-.9.9c0 .5.4.9.9.9H8v2H5.9c-1.1 0-2-.9-2-2zM15 10h-3V8h3c1.1 0 2 .9 2 2s-.9 2-2 2h-3v-2h3c.55 0 1-.45 1-1s-.45-1-1-1zM8.4 11.7l6.3-6.3 1.4 1.4-6.3 6.3-1.4-1.4zm7.2 0l-6.3 6.3-1.4-1.4 6.3-6.3 1.4 1.4z"/>
                              </svg>`;
        shareButton.addEventListener('click', (e) => {
            e.stopPropagation();
            alert(`Share ${item.name}`);
        });

        actionsContainer.appendChild(optionsButton);
        actionsContainer.appendChild(shareButton);

        fileItem.appendChild(iconContainer);
        fileItem.appendChild(fileName);
        fileItem.appendChild(actionsContainer);

        fileBrowser.appendChild(fileItem);
    });
    mainContent.appendChild(fileBrowser);
}

// Handle click on file or folder
function handleItemClick(item, currentPath) {
    if (item.type === 'folder' && item.children) {
        const newPath = [...currentPath, item.name];
        loadFiles(item.children, newPath);
    } else {
        alert(`You selected file: ${item.name}`);
    }
}

// Render breadcrumb with an Up button (if not at root)
function renderBreadcrumb() {
    const container = document.createElement('div');
    container.className = 'breadcrumb-container';

    if (navigationStack.length > 1) {
        const upButton = document.createElement('button');
        upButton.className = 'up-button';
        upButton.title = 'Go Up';
        upButton.textContent = 'Up';
        upButton.addEventListener('click', () => {
            navigateToLevel(navigationStack.length - 2);
        });
        container.appendChild(upButton);
    }

    const breadcrumb = document.createElement('div');
    breadcrumb.className = 'breadcrumb';
    navigationStack.forEach((crumb, index) => {
        const span = document.createElement('span');
        span.textContent = crumb;
        span.addEventListener('click', () => navigateToLevel(index));
        breadcrumb.appendChild(span);
        if (index < navigationStack.length - 1) {
            const separator = document.createTextNode(' / ');
            breadcrumb.appendChild(separator);
        }
    });
    container.appendChild(breadcrumb);

    mainContent.innerHTML = '';
    mainContent.appendChild(container);
}

// Navigate back to a given level based on breadcrumb index
function navigateToLevel(levelIndex) {
    const rootName = navigationStack[0];
    const root = fileSystem.find(r => r.name === rootName);
    let files = root.files;
    const newPath = [rootName];
    for (let i = 1; i <= levelIndex; i++) {
        const folder = files.find(item => item.name === navigationStack[i]);
        if (folder && folder.type === 'folder' && folder.children) {
            files = folder.children;
            newPath.push(folder.name);
        }
    }
    loadFiles(files, newPath);
}

// Attach a simple profile button handler
function attachProfileHandler() {
    profileButton.addEventListener('click', () => {
        alert('Profile settings would appear here.');
    });
}

// Load a complete root (used on initial load)
function loadRoot(root) {
    navigationStack = [root.name];
    loadFiles(root.files, navigationStack);
}

// Returns inline SVG for file icons (modern style)
function getIconHTML(type) {
    if (type === 'folder') {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M10 4H4a2 2 0 00-2 2v12a2 2 0 002 2h16a2 2 0 002-2V8a2 2 0 00-2-2h-8l-2-2z"/>
            </svg>`;
    } else {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z"/>
              <polyline points="14 2 14 8 20 8" fill="none" stroke="var(--primary)" stroke-width="2"/>
            </svg>`;
    }
}