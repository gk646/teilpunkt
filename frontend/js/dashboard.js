import {
    BackendCreateDir,
    BackendCreateFile,
    BackendFetchUserRoots,
    BackendLookupDirectory,
    BackendUploadFile
} from './backend.js';
import {IconFile, IconOptions, IconShare} from './icons.js';

const sideMenu = document.getElementById('sideMenu');
const mainContent = document.getElementById('mainContent');
const profileButton = document.getElementById('profileButton');
const addFileButton = document.getElementById('addFileButton');
const addFileModal = document.getElementById('addFileModal');
const closeModal = document.querySelector('.close');
const addFileForm = document.getElementById('addFileForm');
const menuBar = document.getElementById('menuBar');

let navigationStack = [];


function renderSideMenu(roots) {
    const ul = document.createElement('ul');
    roots.forEach((root, index) => {
        const li = document.createElement('li');
        li.textContent = root.name;
        li.addEventListener('click', () => {
            ul.querySelectorAll('li').forEach(item => item.classList.remove('active'));
            li.classList.add('active');
            navigationStack = [root];
            visitDirectory(root.fid);
        });
        if (index === 0) li.classList.add('active');
        ul.appendChild(li);
    });
    sideMenu.innerHTML = '';
    sideMenu.appendChild(ul);
}

async function visitDirectory(dirID) {
    try {
        const directoryData = await BackendLookupDirectory(dirID);
        renderBreadcrumb();
        renderDirectoryContents(directoryData);
    } catch (error) {
        console.error('Error loading directory:', error);
    }
}

export function renderBreadcrumb() {
    const container = document.getElementById('menuBar');
    const addFileContainer = document.querySelector('.add-file-container');

    // Clear existing breadcrumb content, but preserve the add-file-container
    container.innerHTML = '';

    if (navigationStack.length > 1) {
        const upButton = document.createElement('button');
        upButton.className = 'up-button';
        upButton.textContent = 'Up';
        upButton.addEventListener('click', () => {
            navigationStack.pop();
            visitDirectory(navigationStack[navigationStack.length - 1].fid);
        });
        container.appendChild(upButton);
    }

    const breadcrumb = document.createElement('div');
    breadcrumb.className = 'breadcrumb';

    navigationStack.forEach((entry, index) => {
        const span = document.createElement('span');
        span.textContent = entry.name;
        span.addEventListener('click', () => {
            navigationStack = navigationStack.slice(0, index + 1);
            visitDirectory(entry.fid);
        });
        breadcrumb.appendChild(span);
        span.className = 'breadcrumb-entry'
        if (index < navigationStack.length - 1) {
            breadcrumb.appendChild(document.createTextNode(' / '));
        }
    });

    container.appendChild(breadcrumb);
    if (addFileContainer) {
        container.appendChild(addFileContainer);
    }
}


function renderDirectoryContents(entries) {
    const fileBrowser = document.createElement('div');
    fileBrowser.className = 'file-browser';

    entries.forEach(entry => {
        const fileItem = document.createElement('div');
        fileItem.className = 'file-item';
        fileItem.addEventListener('click', (e) => {
            if (e.target.closest('.file-action')) return;
            if (!entry.isFile) {
                navigationStack.push(entry);
                visitDirectory(entry.fid);
            } else {
                alert(`You selected file: ${entry.name}`);
            }
        });

        const iconContainer = document.createElement('div');
        iconContainer.className = 'file-icon';
        iconContainer.innerHTML = IconFile(entry.isFile);

        const fileName = document.createElement('div');
        fileName.className = 'file-name';
        fileName.textContent = entry.name;

        const actionsContainer = document.createElement('div');
        actionsContainer.className = 'file-actions';

        const optionsButton = document.createElement('button');
        optionsButton.className = 'file-action';
        optionsButton.title = 'Options';
        optionsButton.innerHTML = IconOptions();
        optionsButton.addEventListener('click', (e) => {
            e.stopPropagation();
            alert(`Options for ${entry.name}`);
        });

        const shareButton = document.createElement('button');
        shareButton.className = 'file-action';
        shareButton.title = 'Share';
        shareButton.innerHTML = IconShare();
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
        const roots = await BackendFetchUserRoots();
        renderSideMenu(roots);
        if (roots.length > 0) {
            navigationStack = [roots[0]];
            await visitDirectory(roots[0].fid);
        }
    } catch (error) {
        console.error('Failed to initialize file system UI:', error);
    }
}

function showAddFileModal() {
    addFileModal.style.display = 'block';
}

function hideAddFileModal() {
    addFileModal.style.display = 'none';
}

// Handle form submission for adding a new file or folder
async function handleAddFileFormSubmit(e) {
    e.preventDefault();
    const fileName = document.getElementById('fileName').value;
    const fileType = document.getElementById('fileType').value;

    const lastElement = navigationStack[navigationStack.length - 1];
    try {
        if (fileType === 'folder') {
            await BackendCreateDir(lastElement.fid, fileName);
        } else {
            await BackendCreateFile(lastElement.fid, fileName);
        }

        hideAddFileModal();
        if (navigationStack.length > 0) {
            visitDirectory(navigationStack[navigationStack.length - 1].fid);
        }
    } catch (error) {
        console.error('Error creating file/folder:', error);
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

    document.getElementById('openFilePicker').addEventListener('click', function () {
        document.getElementById('fileUpload').click();
    });

    document.getElementById('fileUpload').addEventListener('change', async function (event) {
        const file = event.target.files[0];
        if (!file) return;
        const currentDir = navigationStack[navigationStack.length - 1].fid;
        await BackendUploadFile(file, currentDir);
        await visitDirectory(currentDir);
    });

});
