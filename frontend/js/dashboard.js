import { fetchRoots, fetchFavourites, fetchDirectory } from './filesystem';
import { getIconHTML } from './icons.js';

// DOM elements
const sideMenu = document.getElementById('sideMenu');
const mainContent = document.getElementById('mainContent');
const profileButton = document.getElementById('profileButton');

// Global navigation state: an array of directory IDs representing the breadcrumb.
let navigationStack = [];

/**
 * Render the side menu using the roots array.
 */
export function renderSideMenu(roots) {
    const ul = document.createElement('ul');
    roots.forEach((root, index) => {
        const li = document.createElement('li');
        li.textContent = root.name; // expects root DTO to have a name property
        li.addEventListener('click', () => {
            // Mark as active.
            ul.querySelectorAll('li').forEach(item => item.classList.remove('active'));
            li.classList.add('active');
            // Reset navigation stack to the root.
            navigationStack = [root.file]; // assuming 'file' is the directory identifier
            loadDirectory(root.file);
        });
        if (index === 0) li.classList.add('active');
        ul.appendChild(li);
    });
    sideMenu.innerHTML = '';
    sideMenu.appendChild(ul);
}

/**
 * Load a directory by its ID.
 * Fetches the directory data and renders the breadcrumb and file list.
 */
export async function loadDirectory(directoryId) {
    try {
        const directoryData = await fetchDirectory(directoryId);
        // Update navigation stack based on the directory data.
        updateNavigationStack(directoryData);
        renderBreadcrumb();
        renderDirectoryContents(directoryData.entries);
    } catch (error) {
        console.error('Error loading directory:', error);
    }
}

/**
 * Update the global navigation stack.
 * This example assumes that the directoryData object has an 'id' property.
 */
function updateNavigationStack(directoryData) {
    // For a new root, we start fresh. Otherwise, navigationStack already contains the path.
    if (navigationStack.length === 0) {
        navigationStack.push(directoryData.id);
    }
    // In a more complete implementation, you might update the navigation based on directoryData.fullPath.
}

/**
 * Render the breadcrumb navigation.
 */
export function renderBreadcrumb() {
    const container = document.createElement('div');
    container.className = 'breadcrumb-container';

    if (navigationStack.length > 1) {
        const upButton = document.createElement('button');
        upButton.className = 'up-button';
        upButton.textContent = 'Up';
        upButton.addEventListener('click', () => {
            // Navigate to the parent directory.
            navigationStack.pop();
            loadDirectory(navigationStack[navigationStack.length - 1]);
        });
        container.appendChild(upButton);
    }

    const breadcrumb = document.createElement('div');
    breadcrumb.className = 'breadcrumb';
    navigationStack.forEach((dirId, index) => {
        const span = document.createElement('span');
        span.textContent = dirId; // In a full implementation, map the ID to a friendly name.
        span.addEventListener('click', () => {
            navigationStack = navigationStack.slice(0, index + 1);
            loadDirectory(dirId);
        });
        breadcrumb.appendChild(span);
        if (index < navigationStack.length - 1) {
            breadcrumb.appendChild(document.createTextNode(' / '));
        }
    });
    container.appendChild(breadcrumb);

    // Insert or replace breadcrumb at the top of mainContent.
    const existingCrumb = document.querySelector('.breadcrumb-container');
    if (existingCrumb) {
        mainContent.replaceChild(container, existingCrumb);
    } else {
        mainContent.insertBefore(container, mainContent.firstChild);
    }
}

/**
 * Render the directory contents (files and subdirectories).
 */
export function renderDirectoryContents(entries) {
    const fileBrowser = document.createElement('div');
    fileBrowser.className = 'file-browser';

    entries.forEach(entry => {
        const fileItem = document.createElement('div');
        fileItem.className = 'file-item';
        fileItem.addEventListener('click', (e) => {
            if (e.target.closest('.file-action')) return;
            if (entry.type === 'folder') {
                navigationStack.push(entry.file); // assuming 'file' is the directory ID.
                loadDirectory(entry.file);
            } else {
                alert(`You selected file: ${entry.name}`);
            }
        });

        // File icon.
        const iconContainer = document.createElement('div');
        iconContainer.className = 'file-icon';
        iconContainer.innerHTML = getIconHTML(entry.type);

        // File name.
        const fileName = document.createElement('div');
        fileName.className = 'file-name';
        fileName.textContent = entry.name;

        // File actions: Options and share buttons.
        const actionsContainer = document.createElement('div');
        actionsContainer.className = 'file-actions';

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
            alert(`Options for ${entry.name}`);
        });

        const shareButton = document.createElement('button');
        shareButton.className = 'file-action';
        shareButton.title = 'Share';
        shareButton.innerHTML = `<svg viewBox="0 0 24 24" width="16" height="16" fill="var(--text)">
                                <path d="M3.9 12c0-1.1.9-2 2-2h3v2H5.9a.9.9 0 00-.9.9c0 .5.4.9.9.9H8v2H5.9c-1.1 0-2-.9-2-2zM15 10h-3V8h3c1.1 0 2 .9 2 2s-.9 2-2 2h-3v-2h3c.55 0 1-.45 1-1s-.45-1-1-1zM8.4 11.7l6.3-6.3 1.4 1.4-6.3 6.3-1.4-1.4zm7.2 0l-6.3 6.3-1.4-1.4 6.3-6.3 1.4 1.4z"/>
                              </svg>`;
        shareButton.addEventListener('click', (e) => {
            e.stopPropagation();
            alert(`Share ${entry.name}`);
        });

        actionsContainer.appendChild(optionsButton);
        actionsContainer.appendChild(shareButton);

        fileItem.appendChild(iconContainer);
        fileItem.appendChild(fileName);
        fileItem.appendChild(actionsContainer);
        fileBrowser.appendChild(fileItem);
    });

    // Clear previous listing but preserve breadcrumb.
    const breadcrumbContainer = document.querySelector('.breadcrumb-container');
    mainContent.innerHTML = '';
    if (breadcrumbContainer) {
        mainContent.appendChild(breadcrumbContainer);
    }
    mainContent.appendChild(fileBrowser);
}


async function initializeFileSystemUI() {
    try {
        // Stage 1: Fetch available roots.
        const roots = await fetchRoots();
        renderSideMenu(roots);
        // Stage 2: Fetch favourites (if needed).
       // const favourites = await fetchFavourites();
        // You could render favourites in a dedicated widget here.
       // console.log('Favourites:', favourites);


        // Stage 3: Load the initial directory from the first root.
        if (roots.length > 0) {
            navigationStack = [roots[0].file];
            loadDirectory(roots[0].file);
        }
    } catch (error) {
        console.error('Failed to initialize file system UI:', error);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    initializeFileSystemUI();
    profileButton.addEventListener('click', () => {
        alert('Profile settings would appear here.');
    });
});