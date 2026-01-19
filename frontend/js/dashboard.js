import {fetchDirectory, fetchRoots} from './filesystem.js';
import {getIconHTML, getOptionsIconHTML, getShareIconHTML} from './icons.js';

// DOM elements
const sideMenu = document.getElementById('sideMenu');
const mainContent = document.getElementById('mainContent');
const profileButton = document.getElementById('profileButton');
const addFileButton = document.getElementById('addFileButton');
const addFileModal = document.getElementById('addFileModal');
const closeModal = document.querySelector('.close');
const addFileForm = document.getElementById('addFileForm');

// Global navigation state: an array of directory IDs representing the breadcrumb.
let navigationStack = [];

// Render the side menu using the roots array.
export function renderSideMenu(roots) {
    const ul = document.createElement('ul');
    roots.forEach((root, index) => {
        const li = document.createElement('li');
        li.textContent = root.name;
        li.addEventListener('click', () => {
            ul.querySelectorAll('li').forEach(item => item.classList.remove('active'));
            li.classList.add('active');
            navigationStack = [root.file];
            loadDirectory(root.file);
        });
        if (index === 0) li.classList.add('active');
        ul.appendChild(li);
    });
    sideMenu.innerHTML = '';
    sideMenu.appendChild(ul);
}

// Load a directory by its ID.
// Fetches the directory data and renders the breadcrumb and file list.
export async function loadDirectory(directoryId) {
    try {
        const directoryData = await fetchDirectory(directoryId);
        updateNavigationStack(directoryData);
        renderBreadcrumb();
        renderDirectoryContents(directoryData.entries);
    } catch (error) {
        console.error('Error loading directory:', error);
    }
}

// Update the global navigation stack.
// This example assumes that the directoryData object has an 'id' property.
function updateNavigationStack(directoryData) {
    if (navigationStack.length === 0) {
        navigationStack.push(directoryData.id);
    }
}

// Render the breadcrumb navigation.
export function renderBreadcrumb() {
    const container = document.createElement('div');
    container.className = 'breadcrumb-container';

    if (navigationStack.length > 1) {
        const upButton = document.createElement('button');
        upButton.className = 'up-button';
        upButton.textContent = 'Up';
        upButton.addEventListener('click', () => {
            navigationStack.pop();
            loadDirectory(navigationStack[navigationStack.length - 1]);
        });
        container.appendChild(upButton);
    }

    const breadcrumb = document.createElement('div');
    breadcrumb.className = 'breadcrumb';
    navigationStack.forEach((dirId, index) => {
        const span = document.createElement('span');
        span.textContent = dirId;
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

    const existingCrumb = document.querySelector('.breadcrumb-container');
    if (existingCrumb) {
        mainContent.replaceChild(container, existingCrumb);
    } else {
        mainContent.insertBefore(container, mainContent.firstChild);
    }
}

// Render the directory contents (files and subdirectories).
export function renderDirectoryContents(entries) {
    const fileBrowser = document.createElement('div');
    fileBrowser.className = 'file-browser';

    entries.forEach(entry => {
        const fileItem = document.createElement('div');
        fileItem.className = 'file-item';
        fileItem.addEventListener('click', (e) => {
            if (e.target.closest('.file-action')) return;
            if (entry.type === 'folder') {
                navigationStack.push(entry.file);
                loadDirectory(entry.file);
            } else {
                alert(`You selected file: ${entry.name}`);
            }
        });

        const iconContainer = document.createElement('div');
        iconContainer.className = 'file-icon';
        iconContainer.innerHTML = getIconHTML(entry.type);

        const fileName = document.createElement('div');
        fileName.className = 'file-name';
        fileName.textContent = entry.name;

        const actionsContainer = document.createElement('div');
        actionsContainer.className = 'file-actions';

        const optionsButton = document.createElement('button');
        optionsButton.className = 'file-action';
        optionsButton.title = 'Options';
        optionsButton.innerHTML = getOptionsIconHTML();
        optionsButton.addEventListener('click', (e) => {
            e.stopPropagation();
            alert(`Options for ${entry.name}`);
        });

        const shareButton = document.createElement('button');
        shareButton.className = 'file-action';
        shareButton.title = 'Share';
        shareButton.innerHTML = getShareIconHTML();
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

    const breadcrumbContainer = document.querySelector('.breadcrumb-container');
    mainContent.innerHTML = '';
    if (breadcrumbContainer) {
        mainContent.appendChild(breadcrumbContainer);
    }
    mainContent.appendChild(fileBrowser);
}

async function initializeFileSystemUI() {
    try {
        const roots = await fetchRoots();
        renderSideMenu(roots);
        if (roots.length > 0) {
            navigationStack = [roots[0].name];
            loadDirectory(roots[0].fid);
        }
    } catch (error) {
        console.error('Failed to initialize file system UI:', error);
    }
}

// Show the add file modal
function showAddFileModal() {
    addFileModal.style.display = 'block';
}

// Hide the add file modal
function hideAddFileModal() {
    addFileModal.style.display = 'none';
}

// Handle form submission for adding a new file or folder
function handleAddFileFormSubmit(e) {
    e.preventDefault();
    const fileName = document.getElementById('fileName').value;
    const fileType = document.getElementById('fileType').value;
    console.log(`Creating new ${fileType}: ${fileName}`);
    hideAddFileModal();
    if (navigationStack.length > 0) {
        loadDirectory(navigationStack[navigationStack.length - 1]);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    initializeFileSystemUI();
    profileButton.addEventListener('click', () => {
        alert('Profile settings would appear here.');
    });

    addFileButton.addEventListener('click', showAddFileModal);
    closeModal.addEventListener('click', hideAddFileModal);
    addFileForm.addEventListener('submit', handleAddFileFormSubmit);

    window.addEventListener('click', (e) => {
        if (e.target === addFileModal) {
            hideAddFileModal();
        }
    });
});
